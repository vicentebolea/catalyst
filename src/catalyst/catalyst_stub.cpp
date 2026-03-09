/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

#include "catalyst_stub.h"
#include "catalyst_conduit.hpp"
#include "catalyst_dump_node.hpp"
#include "catalyst_version.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <future>
#include <thread>

enum catalyst_status catalyst_stub_initialize(const conduit_node* params)
{
  dump_node(params, "initialize", 0, false);
  return catalyst_status_ok;
}

enum catalyst_status catalyst_stub_finalize(const conduit_node* params)
{
  dump_node(params, "finalize", 0, false);
  return catalyst_status_ok;
}

enum catalyst_status catalyst_stub_execute(const conduit_node* params)
{
  static unsigned long invocations = 0;
  dump_node(params, "execute", invocations, true);
  invocations++;

  // Test support: delay execution if requested
  if (conduit_node_has_path(params, "catalyst/test/execute_delay_ms"))
  {
    const conduit_cpp::Node& node = conduit_cpp::cpp_node(const_cast<conduit_node*>(params));
    int delay_ms = node["catalyst/test/execute_delay_ms"].to_int();
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
  }

  // Test support: return error status if requested
  if (conduit_node_has_path(params, "catalyst/test/return_error"))
  {
    return catalyst_status_error_no_implementation;
  }

  // Test support: synchronization for concurrency testing
  if (conduit_node_has_path(params, "catalyst/test/sync/started_promise_ptr"))
  {
    const conduit_cpp::Node& node = conduit_cpp::cpp_node(const_cast<conduit_node*>(params));
    auto ptr_val = node["catalyst/test/sync/started_promise_ptr"].to_int64();
    auto fut_val = node["catalyst/test/sync/continue_future_ptr"].to_int64();

    // NOLINTNEXTLINE(performance-no-int-to-ptr) - test-only code using pointer round-trip
    auto* started = reinterpret_cast<std::promise<void>*>(ptr_val);
    // NOLINTNEXTLINE(performance-no-int-to-ptr) - test-only code using pointer round-trip
    auto* cont = reinterpret_cast<std::future<void>*>(fut_val);

    if (started && cont)
    {
      started->set_value(); // Signal "I started"
      cont->wait();         // Wait for "continue"
    }
  }

  return catalyst_status_ok;
}

enum catalyst_status catalyst_stub_about(conduit_node* params)
{
  conduit_node_set_path_char8_str(params, "catalyst/version", CATALYST_VERSION);
  conduit_node_set_path_char8_str(params, "catalyst/abi_version", CATALYST_ABI_VERSION);
  conduit_node_set_path_char8_str(params, "catalyst/implementation", "stub");
#ifdef CATALYST_USE_MPI
  conduit_node_set_path_int32(params, "catalyst/use_mpi", 1);
#else
  conduit_node_set_path_int32(params, "catalyst/use_mpi", 0);
#endif

  // add information about all third-party dependencies.
  conduit_node* conduit_info = conduit_node_create();
  conduit_about(conduit_info);
  conduit_node_set_path_node(params, "catalyst/tpl/conduit", conduit_info);
  conduit_node_destroy(conduit_info);

  return catalyst_status_ok;
}

enum catalyst_status catalyst_stub_results(conduit_node* /*params*/)
{
  return catalyst_status_ok;
}
