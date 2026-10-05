/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

/*
 * Tests for Catalyst asynchronous execution mode.
 *
 * These tests exercise the async layer through the public API:
 *   - catalyst_initialize / catalyst_execute / catalyst_finalize
 *   - catalyst_about for async status query
 *   - Flush via catalyst_execute with catalyst/async/flush param
 *   - The inline C++ wrapper (catalyst_conduit.hpp)
 */

#include <catalyst.h>
#include <catalyst_conduit.hpp>

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <future>
#include <string>

#ifdef __linux__
#include <pthread.h>
#include <sched.h>
#endif

static size_t g_test_failures = 0;

#define CHECK(cond, msg)                                                                           \
  do                                                                                               \
  {                                                                                                \
    if (!(cond))                                                                                   \
    {                                                                                              \
      fprintf(stderr, "FAIL [%s:%d]: %s\n", __FILE__, __LINE__, (msg));                            \
      g_test_failures++;                                                                           \
    }                                                                                              \
  } while (0)

// ============================================================================
// Helper: Query async enabled status via catalyst_about()
// ============================================================================

static int is_async_enabled()
{
  conduit_cpp::Node about;
  catalyst_about(conduit_cpp::c_node(&about));
  if (about.has_path("catalyst/async/enabled"))
  {
    return about["catalyst/async/enabled"].to_int64() != 0 ? 1 : 0;
  }
  return 0;
}

// ============================================================================
// Helper: Flush async queue via catalyst_execute with flush param
// ============================================================================

static void flush_async()
{
  conduit_cpp::Node params;
  params["catalyst/async/flush"] = 1;
  catalyst_execute(conduit_cpp::c_node(&params));
}

// ============================================================================
// Helper: build a minimal Conduit Blueprint mesh for execute calls
// ============================================================================

static conduit_node* make_test_mesh(int timestep)
{
  conduit_cpp::Node mesh;
  mesh["catalyst/state/timestep"] = timestep;
  mesh["catalyst/state/time"] = static_cast<double>(timestep) * 0.01;
  mesh["catalyst/channels/grid/type"] = "mesh";

  const std::string base = "catalyst/channels/grid/data/coordsets/coords/";
  mesh[base + "type"] = "uniform";
  mesh[base + "dims/i"] = 3;
  mesh[base + "dims/j"] = 3;
  mesh[base + "origin/x"] = 0.0;
  mesh[base + "origin/y"] = 0.0;
  mesh[base + "spacing/dx"] = 1.0;
  mesh[base + "spacing/dy"] = 1.0;

  // Caller receives an owned C node; the C++ wrapper is temporary.
  conduit_node* result = conduit_node_create();
  conduit_node_set_node(result, conduit_cpp::c_node(&mesh));
  return result;
}

// ============================================================================
// Test: async disabled by default
// ============================================================================

static void test_async_disabled_by_default()
{
  const conduit_cpp::Node params;
  // No async config — should initialize in sync mode
  catalyst_initialize(conduit_cpp::c_node(&params));

  CHECK(is_async_enabled() == 0, "Async should be disabled by default");

  const conduit_cpp::Node fin;
  catalyst_finalize(conduit_cpp::c_node(&fin));
}

// ============================================================================
// Test: enable async via params
// ============================================================================

static void test_async_enable_via_params()
{
  conduit_cpp::Node params;
  params["catalyst/async/enabled"] = 1;
  params["catalyst/async/queue_depth"] = 4;
  params["catalyst/async/verbose"] = 0;
  params["catalyst/async/affinity/mode"] = "none";

  catalyst_initialize(conduit_cpp::c_node(&params));

  CHECK(is_async_enabled() == 1, "Async should be enabled via params");

  const conduit_cpp::Node fin;
  catalyst_finalize(conduit_cpp::c_node(&fin));
}

// ============================================================================
// Test: execute and flush
// ============================================================================

static void test_async_execute_and_flush()
{
  conduit_cpp::Node params;
  params["catalyst/async/enabled"] = 1;
  params["catalyst/async/queue_depth"] = 4;
  params["catalyst/async/verbose"] = 0;
  params["catalyst/async/affinity/mode"] = "none";

  catalyst_initialize(conduit_cpp::c_node(&params));
  CHECK(is_async_enabled() == 1, "Async should be enabled");

  // Execute several timesteps
  for (int t = 0; t < 10; t++)
  {
    conduit_node* mesh = make_test_mesh(t);
    catalyst_execute(mesh);
    conduit_node_destroy(mesh);
  }

  // Flush and verify all work completed
  flush_async();

  const conduit_cpp::Node fin;
  catalyst_finalize(conduit_cpp::c_node(&fin));
}

// ============================================================================
// Test: a flush request is not passed to the implementation as a timestep
// ============================================================================

static void test_flush_is_not_executed()
{
  conduit_cpp::Node params;
  params["catalyst/async/enabled"] = 1;
  params["catalyst/async/queue_depth"] = 4;
  params["catalyst/async/verbose"] = 0;
  params["catalyst/async/affinity/mode"] = "none";
  catalyst_initialize(conduit_cpp::c_node(&params));

  for (int t = 0; t < 3; t++)
  {
    conduit_node* mesh = make_test_mesh(t);
    catalyst_execute(mesh);
    conduit_node_destroy(mesh);
  }
  flush_async();

  conduit_cpp::Node about;
  catalyst_about(conduit_cpp::c_node(&about));
  CHECK(about["catalyst/async/stats/timesteps_processed"].to_int64() == 3,
    "A flush should not be executed as a timestep");

  const conduit_cpp::Node fin;
  catalyst_finalize(conduit_cpp::c_node(&fin));
}

// ============================================================================
// Test: a flush waits for queued work even when the queue is full
// ============================================================================

static void test_flush_waits_with_full_queue()
{
  conduit_cpp::Node params;
  params["catalyst/async/enabled"] = 1;
  params["catalyst/async/queue_depth"] = 1;
  params["catalyst/async/verbose"] = 0;
  params["catalyst/async/affinity/mode"] = "none";
  catalyst_initialize(conduit_cpp::c_node(&params));

  // Hold the worker inside the first timestep, then fill the queue.
  std::promise<void> worker_started;
  std::promise<void> continue_signal;
  auto started_future = worker_started.get_future();
  auto continue_future = continue_signal.get_future();

  conduit_cpp::Node mesh0;
  mesh0["catalyst/state/timestep"] = 0;
  mesh0["catalyst/test/sync/started_promise_ptr"] =
    static_cast<int64_t>(reinterpret_cast<intptr_t>(&worker_started));
  mesh0["catalyst/test/sync/continue_future_ptr"] =
    static_cast<int64_t>(reinterpret_cast<intptr_t>(&continue_future));
  catalyst_execute(conduit_cpp::c_node(&mesh0));
  started_future.wait();

  conduit_node* mesh1 = make_test_mesh(1);
  catalyst_execute(mesh1);
  conduit_node_destroy(mesh1);

  auto flushed = std::async(std::launch::async, [] { flush_async(); });
  CHECK(flushed.wait_for(std::chrono::milliseconds(200)) == std::future_status::timeout,
    "Flush returned while timesteps were still queued");

  continue_signal.set_value();
  flushed.wait();

  conduit_cpp::Node about;
  catalyst_about(conduit_cpp::c_node(&about));
  CHECK(about["catalyst/async/stats/timesteps_processed"].to_int64() == 2,
    "Both queued timesteps should be processed when the flush returns");

  const conduit_cpp::Node fin;
  catalyst_finalize(conduit_cpp::c_node(&fin));
}

// ============================================================================
// Test: rapid execute with shallow queue (exercises skip path)
// ============================================================================

static void test_async_rapid_execute()
{
  conduit_cpp::Node params;
  params["catalyst/async/enabled"] = 1;
  params["catalyst/async/queue_depth"] = 1; // Very shallow queue
  params["catalyst/async/verbose"] = 0;
  params["catalyst/async/affinity/mode"] = "none";

  catalyst_initialize(conduit_cpp::c_node(&params));

  // Fire many executes rapidly — with queue_depth=1, some should skip
  for (int t = 0; t < 50; t++)
  {
    conduit_node* mesh = make_test_mesh(t);
    auto status = catalyst_execute(mesh);
    conduit_node_destroy(mesh);
    CHECK(status == catalyst_status_ok, "Execute should always return ok");
  }

  flush_async();

  const conduit_cpp::Node fin;
  catalyst_finalize(conduit_cpp::c_node(&fin));
}

// ============================================================================
// Test: async mode actually runs concurrently
// ============================================================================

static void test_async_is_concurrent()
{
  conduit_cpp::Node params;
  params["catalyst/async/enabled"] = 1;
  params["catalyst/async/queue_depth"] = 2;
  params["catalyst/async/verbose"] = 0;
  params["catalyst/async/affinity/mode"] = "none";

  catalyst_initialize(conduit_cpp::c_node(&params));
  CHECK(is_async_enabled() == 1, "Async should be enabled");

  // Set up synchronization primitives
  std::promise<void> worker_started;
  std::promise<void> continue_signal;
  auto started_future = worker_started.get_future();
  auto continue_future = continue_signal.get_future();

  // Pass pointers to stub via params
  conduit_cpp::Node mesh;
  mesh["catalyst/state/timestep"] = 0;
  mesh["catalyst/test/sync/started_promise_ptr"] =
    static_cast<int64_t>(reinterpret_cast<intptr_t>(&worker_started));
  mesh["catalyst/test/sync/continue_future_ptr"] =
    static_cast<int64_t>(reinterpret_cast<intptr_t>(&continue_future));

  // Execute - should return immediately in async mode
  catalyst_execute(conduit_cpp::c_node(&mesh));

  // Wait for worker to signal it has started (proves worker is running)
  auto status = started_future.wait_for(std::chrono::seconds(5));
  CHECK(status == std::future_status::ready, "Worker should have started");

  // At this point: main thread continued, worker is blocked waiting for us
  // This proves true concurrency

  // Signal worker to continue
  continue_signal.set_value();

  flush_async(); // Wait for worker to finish

  const conduit_cpp::Node fin;
  catalyst_finalize(conduit_cpp::c_node(&fin));
}

// ============================================================================
// Test: slow execution tracking
// ============================================================================

static void test_async_slow_tracking()
{
  conduit_cpp::Node params;
  params["catalyst/async/enabled"] = 1;
  params["catalyst/async/queue_depth"] = 2;
  params["catalyst/async/verbose"] = 0;
  params["catalyst/async/affinity/mode"] = "none";
  params["catalyst/async/slow_threshold"] = 0.05; // 50ms threshold

  catalyst_initialize(conduit_cpp::c_node(&params));

  // Execute with 100ms delay - should trigger slow warning
  conduit_cpp::Node mesh;
  mesh["catalyst/state/timestep"] = 0;
  mesh["catalyst/test/execute_delay_ms"] = 100;

  catalyst_execute(conduit_cpp::c_node(&mesh));
  flush_async();

  // Test passes if no crash - slow count logged in verbose mode
  const conduit_cpp::Node fin;
  catalyst_finalize(conduit_cpp::c_node(&fin));
}

// ============================================================================
// Test: error status tracking
// ============================================================================

static void test_async_error_tracking()
{
  conduit_cpp::Node params;
  params["catalyst/async/enabled"] = 1;
  params["catalyst/async/queue_depth"] = 2;
  params["catalyst/async/verbose"] = 0;
  params["catalyst/async/affinity/mode"] = "none";

  catalyst_initialize(conduit_cpp::c_node(&params));

  // Execute normally first
  conduit_cpp::Node mesh1;
  mesh1["catalyst/state/timestep"] = 0;
  catalyst_execute(conduit_cpp::c_node(&mesh1));

  // Execute with error return
  conduit_cpp::Node mesh2;
  mesh2["catalyst/state/timestep"] = 1;
  mesh2["catalyst/test/return_error"] = 1;
  catalyst_execute(conduit_cpp::c_node(&mesh2));

  // Execute normally after error
  conduit_cpp::Node mesh3;
  mesh3["catalyst/state/timestep"] = 2;
  catalyst_execute(conduit_cpp::c_node(&mesh3));

  flush_async();

  // Worker should have continued after error - test passes if no crash
  const conduit_cpp::Node fin;
  catalyst_finalize(conduit_cpp::c_node(&fin));
}

// ============================================================================
// Test: exception handling
// ============================================================================

// ============================================================================
// Test: synchronous fallback when disabled
// ============================================================================

static void test_sync_fallback()
{
  conduit_cpp::Node params;
  params["catalyst/async/enabled"] = 0;

  catalyst_initialize(conduit_cpp::c_node(&params));
  CHECK(is_async_enabled() == 0, "Async should be disabled");

  for (int t = 0; t < 5; t++)
  {
    conduit_node* mesh = make_test_mesh(t);
    auto status = catalyst_execute(mesh);
    conduit_node_destroy(mesh);
    CHECK(status == catalyst_status_ok, "Sync execute should return ok");
  }

  // Flush is a no-op when disabled
  flush_async();

  const conduit_cpp::Node fin;
  catalyst_finalize(conduit_cpp::c_node(&fin));
}

// ============================================================================
// Test: finalize drains pending work
// ============================================================================

static void test_finalize_drains_queue()
{
  conduit_cpp::Node params;
  params["catalyst/async/enabled"] = 1;
  params["catalyst/async/queue_depth"] = 4;
  params["catalyst/async/verbose"] = 0;
  params["catalyst/async/affinity/mode"] = "none";

  catalyst_initialize(conduit_cpp::c_node(&params));

  // Set up synchronization to ensure work is queued
  std::promise<void> worker_started;
  std::promise<void> continue_signal;
  auto started_future = worker_started.get_future();
  auto continue_future = continue_signal.get_future();

  // First execute with sync - worker will block waiting for signal
  conduit_cpp::Node mesh1;
  mesh1["catalyst/state/timestep"] = 0;
  mesh1["catalyst/test/sync/started_promise_ptr"] =
    static_cast<int64_t>(reinterpret_cast<intptr_t>(&worker_started));
  mesh1["catalyst/test/sync/continue_future_ptr"] =
    static_cast<int64_t>(reinterpret_cast<intptr_t>(&continue_future));
  catalyst_execute(conduit_cpp::c_node(&mesh1));

  // Wait for worker to start processing first item
  started_future.wait();

  // Now enqueue more work while worker is blocked
  for (int t = 1; t < 5; t++)
  {
    conduit_node* mesh = make_test_mesh(t);
    catalyst_execute(mesh);
    conduit_node_destroy(mesh);
  }

  // Release worker and finalize (which drains the queue)
  continue_signal.set_value();

  const conduit_cpp::Node fin;
  catalyst_finalize(conduit_cpp::c_node(&fin));

  // Verify stats via catalyst_about() AFTER finalize
  // (impl pointer stays valid, stats persist in g_ctx)
  conduit_cpp::Node about;
  catalyst_about(conduit_cpp::c_node(&about));
  auto processed = about["catalyst/async/stats/timesteps_processed"].to_int64();
  CHECK(processed == 5, "All 5 timesteps should be processed after finalize drains queue");

  CHECK(is_async_enabled() == 0, "Async disabled after finalize");
}

// ============================================================================
// Test: re-initialization cycle
// ============================================================================

static void test_reinitialize_cycle()
{
  conduit_cpp::Node params;
  params["catalyst/async/enabled"] = 1;
  params["catalyst/async/queue_depth"] = 2;
  params["catalyst/async/verbose"] = 0;
  params["catalyst/async/affinity/mode"] = "none";

  // First cycle
  catalyst_initialize(conduit_cpp::c_node(&params));
  CHECK(is_async_enabled() == 1, "Async enabled in first cycle");

  conduit_node* mesh = make_test_mesh(0);
  catalyst_execute(mesh);
  conduit_node_destroy(mesh);
  flush_async();

  const conduit_cpp::Node fin;
  catalyst_finalize(conduit_cpp::c_node(&fin));
  CHECK(is_async_enabled() == 0, "Async disabled after first finalize");

  // Second cycle
  catalyst_initialize(conduit_cpp::c_node(&params));
  CHECK(is_async_enabled() == 1, "Async enabled in second cycle");

  for (int t = 0; t < 3; t++)
  {
    conduit_node* m = make_test_mesh(t);
    catalyst_execute(m);
    conduit_node_destroy(m);
  }
  flush_async();

  const conduit_cpp::Node fin2;
  catalyst_finalize(conduit_cpp::c_node(&fin2));
}

// ============================================================================
// Test: auto affinity mode (exercises topology detection + platform pinning)
// ============================================================================

static void test_async_affinity_auto()
{
  conduit_cpp::Node params;
  params["catalyst/async/enabled"] = 1;
  params["catalyst/async/queue_depth"] = 2;
  params["catalyst/async/verbose"] = 1;
  params["catalyst/async/affinity/mode"] = "auto";

  catalyst_initialize(conduit_cpp::c_node(&params));
  CHECK(is_async_enabled() == 1, "Async should be enabled with auto affinity");

  // Execute something to ensure worker thread has started and applied affinity
  conduit_node* mesh = make_test_mesh(0);
  catalyst_execute(mesh);
  conduit_node_destroy(mesh);
  flush_async();

  // Verify affinity info via catalyst_about()
  conduit_cpp::Node info;
  catalyst_about(conduit_cpp::c_node(&info));

  CHECK(info.has_path("catalyst/async"), "catalyst_about should include async info");
  CHECK(info["catalyst/async/affinity_mode"].as_string() == "auto", "Affinity mode should be auto");
  CHECK(info.has_path("catalyst/async/hwloc_available"), "Should report hwloc availability");
  CHECK(info.has_path("catalyst/async/worker_pinned_core"), "Should report pinned core");

  // Check if pinning actually occurred
  const int pinned_core = info["catalyst/async/worker_pinned_core"].to_int();

  // On Linux/Windows, affinity should work. On macOS, it may fail even with hwloc.
#if defined(__linux__) || defined(_WIN32)
  CHECK(pinned_core >= 0, "Worker should be pinned to a core on Linux/Windows");
#else
  // On macOS: just verify we get a valid response (-1 = not pinned, >= 0 = pinned)
  // Don't fail if pinning didn't work - macOS has limited affinity support
  (void)pinned_core;
#endif

  const conduit_cpp::Node fin;
  catalyst_finalize(conduit_cpp::c_node(&fin));
}

// ============================================================================
// Test: manual affinity mode (exercises explicit core assignment)
// ============================================================================

static void test_async_affinity_manual()
{
  // Detect a core that this process is actually allowed to use, since CI
  // environments (e.g. PowerPC) may not include core 0 in the cpuset.
  int target_core = 0;
#ifdef __linux__
  cpu_set_t allowed;
  CPU_ZERO(&allowed);
  if (sched_getaffinity(0, sizeof(allowed), &allowed) == 0)
  {
    for (int i = 0; i < CPU_SETSIZE; i++)
    {
      if (CPU_ISSET(i, &allowed))
      {
        target_core = i;
        break;
      }
    }
  }
#endif

  conduit_cpp::Node params;
  params["catalyst/async/enabled"] = 1;
  params["catalyst/async/queue_depth"] = 2;
  params["catalyst/async/verbose"] = 1;
  params["catalyst/async/affinity/mode"] = "manual";
  params["catalyst/async/affinity/worker_cores"].append().set(target_core);

  catalyst_initialize(conduit_cpp::c_node(&params));
  CHECK(is_async_enabled() == 1, "Async should be enabled with manual affinity");

  // Execute something to ensure worker thread has started and applied affinity
  conduit_node* mesh = make_test_mesh(0);
  catalyst_execute(mesh);
  conduit_node_destroy(mesh);
  flush_async();

  // Verify affinity info via catalyst_about()
  conduit_cpp::Node info;
  catalyst_about(conduit_cpp::c_node(&info));

  CHECK(
    info["catalyst/async/affinity_mode"].as_string() == "manual", "Affinity mode should be manual");

  // Check that pinning occurred to the requested core
  const int pinned_core = info["catalyst/async/worker_pinned_core"].to_int();

#if defined(__linux__) || defined(_WIN32)
  CHECK(
    pinned_core == target_core, "Worker should be pinned to the requested core on Linux/Windows");
#else
  // On macOS: hwloc can detect topology but cannot set thread affinity.
  // Pinning will fail even with hwloc available, so just verify the value is reported.
  (void)pinned_core;
#endif

  const conduit_cpp::Node fin;
  catalyst_finalize(conduit_cpp::c_node(&fin));
}

// ============================================================================
// Test: GPU runtime detection reporting
// ============================================================================

static void test_gpu_runtime_reporting()
{
  conduit_cpp::Node params;
  params["catalyst/async/enabled"] = 1;
  params["catalyst/async/verbose"] = 0;
  params["catalyst/async/affinity/mode"] = "none";

  catalyst_initialize(conduit_cpp::c_node(&params));

  // Query async info
  conduit_cpp::Node info;
  catalyst_about(conduit_cpp::c_node(&info));

  // gpu_runtime should be reported (none, cuda, or hip)
  CHECK(info.has_path("catalyst/async/gpu_runtime"), "gpu_runtime should be reported");
  const std::string gpu_runtime = info["catalyst/async/gpu_runtime"].as_string();

  // On CI without GPU, should be "none"
  // On a system with CUDA/HIP, could be "cuda" or "hip"
  CHECK(gpu_runtime == "none" || gpu_runtime == "cuda" || gpu_runtime == "hip",
    "gpu_runtime should be none, cuda, or hip");

  const conduit_cpp::Node fin;
  catalyst_finalize(conduit_cpp::c_node(&fin));
}

// ============================================================================
// Main
// ============================================================================

// NOLINTNEXTLINE(bugprone-exception-escape) - test code using std::promise/future
int main(int /*argc*/, char* /*argv*/[])
{
  fprintf(stderr, "=== Catalyst Async Tests ===\n\n");

  fprintf(stderr, "--- test_async_disabled_by_default ---\n");
  test_async_disabled_by_default();

  fprintf(stderr, "--- test_async_enable_via_params ---\n");
  test_async_enable_via_params();

  fprintf(stderr, "--- test_async_execute_and_flush ---\n");
  test_async_execute_and_flush();

  fprintf(stderr, "--- test_flush_is_not_executed ---\n");
  test_flush_is_not_executed();

  fprintf(stderr, "--- test_flush_waits_with_full_queue ---\n");
  test_flush_waits_with_full_queue();

  fprintf(stderr, "--- test_async_rapid_execute ---\n");
  test_async_rapid_execute();

  fprintf(stderr, "--- test_async_is_concurrent ---\n");
  test_async_is_concurrent();

  fprintf(stderr, "--- test_async_slow_tracking ---\n");
  test_async_slow_tracking();

  fprintf(stderr, "--- test_async_error_tracking ---\n");
  test_async_error_tracking();

  fprintf(stderr, "--- test_sync_fallback ---\n");
  test_sync_fallback();

  fprintf(stderr, "--- test_finalize_drains_queue ---\n");
  test_finalize_drains_queue();

  fprintf(stderr, "--- test_reinitialize_cycle ---\n");
  test_reinitialize_cycle();

  fprintf(stderr, "--- test_async_affinity_auto ---\n");
  test_async_affinity_auto();

  fprintf(stderr, "--- test_async_affinity_manual ---\n");
  test_async_affinity_manual();

  fprintf(stderr, "--- test_gpu_runtime_reporting ---\n");
  test_gpu_runtime_reporting();

  fprintf(stderr, "\n=== Results: %zu failure(s) ===\n", g_test_failures);

  return g_test_failures > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
