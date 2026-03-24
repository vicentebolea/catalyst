/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

#include "catalyst_async.h"

#include <conduit.hpp>
#include <conduit_cpp_to_c.hpp>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <cstring>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#ifdef CATALYST_USE_MPI
#include <mpi.h>
#endif

#ifdef __linux__
#include <pthread.h>
#include <sched.h>
#elif defined(_WIN32)
#define NOMINMAX
#include <windows.h>
#endif

#if CATALYST_ASYNC_USE_HWLOC
#include <hwloc.h>
#endif

// For GPU runtime detection via dlopen (Linux)
#ifndef _WIN32
#include <dlfcn.h>
#endif

namespace
{

// ============================================================================
// Constants
// ============================================================================

inline constexpr size_t kDefaultQueueDepth = 2;
inline constexpr std::array kLocalRankEnvVars = {
  "OMPI_COMM_WORLD_LOCAL_RANK", // OpenMPI
  "MV2_COMM_WORLD_LOCAL_RANK",  // MVAPICH2
  "SLURM_LOCALID",              // SLURM
  "MPI_LOCALRANKID",            // Intel MPI
  "PMI_LOCAL_RANK",             // MPICH/Cray
};

inline constexpr std::array kLocalSizeEnvVars = {
  "OMPI_COMM_WORLD_LOCAL_SIZE",
  "MV2_COMM_WORLD_LOCAL_SIZE",
  "SLURM_NTASKS_PER_NODE",
};

// ============================================================================
// Configuration
// ============================================================================

struct AsyncConfig
{
  bool enabled = false;
  size_t queue_depth = kDefaultQueueDepth;
  bool verbose = false;
  double slow_threshold_sec = 10.0; // Log warning if execute takes longer
  double flush_timeout_sec = 300.0; // 5 minutes default, 0 = no timeout

  enum class AffinityMode : std::uint8_t
  {
    Auto,
    Manual,
    None
  };
  AffinityMode affinity_mode = AffinityMode::Auto;
  std::vector<int> worker_cores;
};

// ============================================================================
// Work item & Statistics
// ============================================================================

struct WorkItem
{
  enum class Type : std::uint8_t
  {
    EXECUTE,
    INITIALIZE,
    FINALIZE
  };

  Type type = Type::EXECUTE;
  conduit::Node data;
  std::chrono::steady_clock::time_point enqueue_time;

  // For INITIALIZE and FINALIZE: function to call + promise for sync return.
  // nullptr for EXECUTE items.
  enum catalyst_status (*impl_fn)(const conduit_node*) = nullptr;
  std::shared_ptr<std::promise<enum catalyst_status>> sync_promise;
};

struct AsyncStats
{
  std::atomic<size_t> timesteps_processed{ 0 };
  std::atomic<size_t> timesteps_skipped{ 0 };
  std::atomic<size_t> execute_errors{ 0 };
  std::atomic<size_t> timesteps_slow{ 0 };
  size_t max_queue_depth_seen = 0;
  double total_copy_time = 0.0;
  double total_execute_time = 0.0;
  double max_queue_wait = 0.0;
  double max_execute_time = 0.0;
};

// ============================================================================
// GPU runtime detection (dlopen-based, no build-time dependency)
// Uses CUDA Driver API (nvcuda.dll / libcuda.so) instead of Runtime API.
// The driver API is version-stable and always present when NVIDIA drivers
// are installed, avoiding version-specific DLL names like cudart64_12.dll.
// For HIP, we use the runtime API since HIP doesn't have a separate driver API.
// ============================================================================

struct GpuRuntime
{
  enum class Backend : std::uint8_t
  {
    None,
    CUDA,
    HIP
  };

  Backend backend = Backend::None;

#ifdef _WIN32
  HMODULE handle = nullptr;
#else
  void* handle = nullptr;
#endif

  // CUDA Driver API function pointer types
  // cuInit(unsigned int Flags) -> CUresult
  using CuInitFn = int (*)(unsigned int);

  // cuPointerGetAttribute(void* data, CUpointer_attribute attribute, CUdeviceptr devPtr)
  // Note: CUdeviceptr is unsigned long long on 64-bit
  using CuPointerGetAttributeFn = int (*)(void*, int, unsigned long long);

  // cuMemcpyDtoH(void* dstHost, CUdeviceptr srcDevice, size_t ByteCount)
  using CuMemcpyDtoHFn = int (*)(void*, unsigned long long, size_t);

  // HIP Runtime API function pointer types (HIP doesn't have separate driver API)
  // hipPointerGetAttributes(hipPointerAttribute_t* attributes, const void* ptr)
  using HipPointerGetAttributesFn = int (*)(void*, const void*);

  // hipMemcpy(void* dst, const void* src, size_t sizeBytes, hipMemcpyKind kind)
  using HipMemcpyFn = int (*)(void*, const void*, size_t, int);

  // Cached function pointers - CUDA driver API
  CuPointerGetAttributeFn cuPointerGetAttribute = nullptr;
  CuMemcpyDtoHFn cuMemcpyDtoH = nullptr;

  // Cached function pointers - HIP runtime API
  HipPointerGetAttributesFn hipPointerGetAttributes = nullptr;
  HipMemcpyFn hipMemcpy = nullptr;

  // CUDA Driver API constants
  // CU_POINTER_ATTRIBUTE_MEMORY_TYPE = 2
  static constexpr int CU_POINTER_ATTRIBUTE_MEMORY_TYPE = 2;
  // CU_MEMORYTYPE_DEVICE = 2, CU_MEMORYTYPE_UNIFIED = 4
  static constexpr unsigned int CU_MEMORYTYPE_DEVICE = 2;
  static constexpr unsigned int CU_MEMORYTYPE_UNIFIED = 4;

  // HIP Runtime API constants (from hip_runtime_api.h)
  // Note: HIP uses different values than CUDA runtime!
  // hipMemoryTypeHost = 0, hipMemoryTypeDevice = 1, hipMemoryTypeManaged = 3
  static constexpr int HIP_MEMORY_TYPE_HOST = 0;
  static constexpr int HIP_MEMORY_TYPE_DEVICE = 1;
  static constexpr int HIP_MEMORY_TYPE_MANAGED = 3;
  // hipMemcpyDeviceToHost = 2
  static constexpr int HIP_MEMCPY_DEVICE_TO_HOST = 2;
};

// ============================================================================
// Global state - consolidated into a single context structure
// ============================================================================

struct CatalystAsyncContext
{
  AsyncConfig config;
  AsyncStats stats;
  GpuRuntime gpu_runtime;

  std::thread worker_thread;
  std::mutex queue_mutex;
  std::mutex impl_mutex; // Guards all impl->xxx() calls
  std::condition_variable queue_cv;
  std::queue<WorkItem> work_queue;
  std::atomic<bool> should_stop{ false };
  std::atomic<bool> worker_busy{ false };
  std::atomic<bool> initialized{ false };
  std::atomic<bool> initializing{ false };

  enum catalyst_status (*impl_execute)(const conduit_node*) = nullptr;

  // Worker thread affinity tracking
  std::atomic<int> worker_pinned_core{ -1 }; // -1 = not pinned

#ifdef CATALYST_USE_MPI
  MPI_Comm mpi_comm = MPI_COMM_NULL;
#endif
};

CatalystAsyncContext g_ctx;

#ifdef CATALYST_USE_MPI
// Check if MPI is initialized and not yet finalized
[[nodiscard]] bool mpi_is_usable()
{
  int initialized = 0;
  int finalized = 0;
  if (MPI_Initialized(&initialized) != MPI_SUCCESS)
  {
    return false;
  }
  if (initialized)
  {
    if (MPI_Finalized(&finalized) != MPI_SUCCESS)
    {
      return false;
    }
  }
  return initialized && !finalized;
}
#endif

// ============================================================================
// GPU runtime initialization (dlopen-based)
// ============================================================================

void init_gpu_runtime()
{
  // Already initialized?
  if (g_ctx.gpu_runtime.backend != GpuRuntime::Backend::None)
  {
    return;
  }

#ifdef _WIN32
  // Use CUDA Driver API (nvcuda.dll) - version-stable, always present with NVIDIA drivers
  HMODULE cuda_handle = GetModuleHandleA("nvcuda.dll");
  if (cuda_handle)
  {
    auto cuInit = (GpuRuntime::CuInitFn)GetProcAddress(cuda_handle, "cuInit");
    auto cuPtrGetAttr =
      (GpuRuntime::CuPointerGetAttributeFn)GetProcAddress(cuda_handle, "cuPointerGetAttribute");
    auto cuMemcpyDtoH = (GpuRuntime::CuMemcpyDtoHFn)GetProcAddress(cuda_handle, "cuMemcpyDtoH_v2");

    if (cuInit && cuPtrGetAttr && cuMemcpyDtoH)
    {
      // cuInit may have already been called by the simulation; that's fine
      cuInit(0);
      g_ctx.gpu_runtime.handle = cuda_handle;
      g_ctx.gpu_runtime.cuPointerGetAttribute = cuPtrGetAttr;
      g_ctx.gpu_runtime.cuMemcpyDtoH = cuMemcpyDtoH;
      g_ctx.gpu_runtime.backend = GpuRuntime::Backend::CUDA;
      if (g_ctx.config.verbose)
      {
        std::cerr << "catalyst_async: CUDA driver API detected (Windows)\n";
      }
      return;
    }
  }

  // Try HIP (AMD) - amdhip64.dll - use runtime API (HIP has no separate driver API)
  HMODULE hip_handle = GetModuleHandleA("amdhip64.dll");
  if (hip_handle)
  {
    auto hipPtrGetAttrs =
      (GpuRuntime::HipPointerGetAttributesFn)GetProcAddress(hip_handle, "hipPointerGetAttributes");
    auto hipMemcpyFn = (GpuRuntime::HipMemcpyFn)GetProcAddress(hip_handle, "hipMemcpy");

    if (hipPtrGetAttrs && hipMemcpyFn)
    {
      g_ctx.gpu_runtime.handle = hip_handle;
      g_ctx.gpu_runtime.hipPointerGetAttributes = hipPtrGetAttrs;
      g_ctx.gpu_runtime.hipMemcpy = hipMemcpyFn;
      g_ctx.gpu_runtime.backend = GpuRuntime::Backend::HIP;
      if (g_ctx.config.verbose)
      {
        std::cerr << "catalyst_async: HIP runtime detected (Windows)\n";
      }
      return;
    }
  }
#elif defined(__linux__)
  // Use CUDA Driver API (libcuda.so) - version-stable
  // RTLD_NOLOAD returns a handle to libcuda.so if it is already loaded by any
  // means (including RTLD_LOCAL linkage from nvcc-compiled code), without
  // loading it if absent. This is the correct analog to GetModuleHandleA on
  // Windows: we don't want to force-load CUDA into a non-CUDA process.

  void* cuda_handle = dlopen("libcuda.so", RTLD_NOW | RTLD_NOLOAD);

  if (cuda_handle)
  {
    auto cuInit = reinterpret_cast<GpuRuntime::CuInitFn>(dlsym(cuda_handle, "cuInit"));
    auto cuPtrGetAttr = reinterpret_cast<GpuRuntime::CuPointerGetAttributeFn>(
      dlsym(cuda_handle, "cuPointerGetAttribute"));
    auto cuMemcpyDtoH =
      reinterpret_cast<GpuRuntime::CuMemcpyDtoHFn>(dlsym(cuda_handle, "cuMemcpyDtoH_v2"));

    if (cuInit && cuPtrGetAttr && cuMemcpyDtoH)
    {
      cuInit(0);
      g_ctx.gpu_runtime.handle = nullptr;
      g_ctx.gpu_runtime.cuPointerGetAttribute = cuPtrGetAttr;
      g_ctx.gpu_runtime.cuMemcpyDtoH = cuMemcpyDtoH;
      g_ctx.gpu_runtime.backend = GpuRuntime::Backend::CUDA;
      if (g_ctx.config.verbose)
      {
        std::cerr << "catalyst_async: CUDA driver API detected\n";
      }
      dlclose(cuda_handle);
      return;
    }
  }

  // Try HIP (AMD) - use runtime API (HIP has no separate driver API)
  auto hipPtrGetAttrs = reinterpret_cast<GpuRuntime::HipPointerGetAttributesFn>(
    dlsym(RTLD_DEFAULT, "hipPointerGetAttributes"));
  auto hipMemcpyFn = reinterpret_cast<GpuRuntime::HipMemcpyFn>(dlsym(RTLD_DEFAULT, "hipMemcpy"));

  if (hipPtrGetAttrs && hipMemcpyFn)
  {
    g_ctx.gpu_runtime.handle = nullptr;
    g_ctx.gpu_runtime.hipPointerGetAttributes = hipPtrGetAttrs;
    g_ctx.gpu_runtime.hipMemcpy = hipMemcpyFn;
    g_ctx.gpu_runtime.backend = GpuRuntime::Backend::HIP;
    if (g_ctx.config.verbose)
    {
      std::cerr << "catalyst_async: HIP runtime detected\n";
    }
    return;
  }
#else
  // macOS/other: No CUDA or HIP available
  // (Apple uses Metal which has a different memory model)
#endif

  // No GPU runtime found - that's fine, assume all pointers are CPU accessible
  if (g_ctx.config.verbose)
  {
    std::cerr << "catalyst_async: No GPU runtime detected, assuming CPU-accessible memory\n";
  }
}

// Check if a pointer is a GPU device pointer
// Returns true if GPU runtime is available and pointer is device memory
[[nodiscard]] bool is_device_pointer(const void* ptr)
{
  if (g_ctx.gpu_runtime.backend == GpuRuntime::Backend::None || !ptr)
  {
    return false;
  }

  if (g_ctx.gpu_runtime.backend == GpuRuntime::Backend::CUDA)
  {
    // CUDA Driver API: cuPointerGetAttribute with CU_POINTER_ATTRIBUTE_MEMORY_TYPE
    // Returns CU_MEMORYTYPE_DEVICE (2) or CU_MEMORYTYPE_UNIFIED (4) for device memory
    unsigned int memType = 0;
    const int result = g_ctx.gpu_runtime.cuPointerGetAttribute(&memType,
      GpuRuntime::CU_POINTER_ATTRIBUTE_MEMORY_TYPE, reinterpret_cast<unsigned long long>(ptr));

    if (result != 0)
    {
      return false;
    }
    return (
      memType == GpuRuntime::CU_MEMORYTYPE_DEVICE || memType == GpuRuntime::CU_MEMORYTYPE_UNIFIED);
  }

  // HIP Runtime API: hipPointerGetAttributes returns hipPointerAttribute_t
  // Struct layout from hip_runtime_api.h:
  //   enum hipMemoryType memoryType;
  //   int device;
  //   void* devicePointer;
  //   void* hostPointer;
  //   int isManaged;
  //   unsigned allocationFlags;
  struct HipPointerAttribute
  {
    int memoryType;
    int device;
    void* devicePointer;
    void* hostPointer;
    int isManaged;
  };

  HipPointerAttribute attrs = {};
  const int result = g_ctx.gpu_runtime.hipPointerGetAttributes(&attrs, ptr);

  if (result != 0)
  {
    return false;
  }
  return (attrs.memoryType == GpuRuntime::HIP_MEMORY_TYPE_DEVICE || attrs.isManaged != 0);
}

// Temporary buffer storage for GPU->CPU copies (freed after compact_to)
struct GpuCopyBuffer
{
  std::vector<char> data;
};
thread_local std::vector<GpuCopyBuffer>
  g_gpu_copy_buffers; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

// Recursively walk a Conduit node and copy any GPU arrays to CPU
// Modifies the node in-place to point to CPU copies
// NOLINTNEXTLINE(misc-no-recursion) - recursive tree walk is intentional
void copy_gpu_arrays_to_cpu_impl(conduit::Node& node)
{
  if (node.dtype().is_empty())
  {
    return;
  }

  // Check if this is a leaf with external data
  if (node.dtype().is_number() && node.is_data_external())
  {
    const void* ptr = node.data_ptr();
    if (is_device_pointer(ptr))
    {
      const size_t nbytes = node.dtype().bytes_compact();

      // Allocate CPU buffer
      g_gpu_copy_buffers.emplace_back();
      auto& buf = g_gpu_copy_buffers.back();
      buf.data.resize(nbytes);

      // Copy GPU -> CPU
      int result = 0;
      if (g_ctx.gpu_runtime.backend == GpuRuntime::Backend::CUDA)
      {
        // CUDA Driver API: cuMemcpyDtoH(dst, src, bytes)
        result = g_ctx.gpu_runtime.cuMemcpyDtoH(
          buf.data.data(), reinterpret_cast<unsigned long long>(ptr), nbytes);
      }
      else
      {
        // HIP Runtime API: hipMemcpy(dst, src, bytes, hipMemcpyDeviceToHost)
        result = g_ctx.gpu_runtime.hipMemcpy(
          buf.data.data(), ptr, nbytes, GpuRuntime::HIP_MEMCPY_DEVICE_TO_HOST);
      }

      if (result == 0)
      {
        // Update node to point to CPU copy
        // We need to use set_external with the same dtype
        const conduit::DataType saved_dtype = node.dtype();
        node.set_external(saved_dtype, buf.data.data());

        if (g_ctx.config.verbose)
        {
          std::cerr << "catalyst_async: Copied " << nbytes << " bytes from GPU to CPU\n";
        }
      }
      else
      {
        std::cerr << "catalyst_async: GPU->CPU copy failed for " << nbytes << " bytes\n";
      }
    }
  }
  else if (node.dtype().is_object() || node.dtype().is_list())
  {
    // Recurse into children
    for (conduit::index_t i = 0; i < node.number_of_children(); i++)
    {
      copy_gpu_arrays_to_cpu_impl(node.child(i));
    }
  }
}

// Main entry point for GPU->CPU copy
// Call before compact_to(), clears buffers after use
void copy_gpu_arrays_to_cpu(conduit::Node& node)
{
  if (g_ctx.gpu_runtime.backend == GpuRuntime::Backend::None)
  {
    return; // No GPU runtime, assume all pointers are CPU
  }

  // Clear any previous buffers
  g_gpu_copy_buffers.clear();

  // Walk the tree and copy GPU arrays
  copy_gpu_arrays_to_cpu_impl(node);
}

// Clear GPU copy buffers (call after compact_to completes)
void clear_gpu_copy_buffers()
{
  g_gpu_copy_buffers.clear();
}

// ============================================================================
// Helper functions
// ============================================================================

// ============================================================================
// Environment variable helpers (templated)
// ============================================================================

[[nodiscard]] std::optional<std::string> get_env_string(const char* name)
{
  if (!name)
  {
    return std::nullopt;
  }
  if (const char* val = std::getenv(name); val)
  {
    return std::string(val);
  }
  return std::nullopt;
}

template <typename T>
[[nodiscard]] std::optional<T> get_env(const char* name);

template <>
[[nodiscard]] std::optional<std::string> get_env<std::string>(const char* name)
{
  return get_env_string(name);
}

template <>
[[nodiscard]] std::optional<bool> get_env<bool>(const char* name)
{
  auto val = get_env_string(name);
  if (!val || val->empty())
  {
    return std::nullopt;
  }
  return *val == "1" || *val == "true" || *val == "TRUE" || *val == "yes";
}

template <>
[[nodiscard]] std::optional<int> get_env<int>(const char* name)
{
  auto val = get_env_string(name);
  if (!val || val->empty())
  {
    return std::nullopt;
  }
  try
  {
    return std::stoi(*val);
  }
  catch (...)
  {
    std::cerr << "catalyst_async: Invalid integer value for " << name << ": '" << *val << "'\n";
    return std::nullopt;
  }
}

template <>
[[nodiscard]] std::optional<size_t> get_env<size_t>(const char* name)
{
  auto val = get_env_string(name);
  if (!val || val->empty())
  {
    return std::nullopt;
  }
  // Reject negative values (stoull would wrap them)
  // Check after skipping whitespace since stoull does too
  auto pos = val->find_first_not_of(" \t");
  if (pos != std::string::npos && (*val)[pos] == '-')
  {
    std::cerr << "catalyst_async: Invalid size value for " << name << ": '" << *val
              << "' (must be non-negative)\n";
    return std::nullopt;
  }
  try
  {
    return std::stoull(*val);
  }
  catch (...)
  {
    std::cerr << "catalyst_async: Invalid size value for " << name << ": '" << *val << "'\n";
    return std::nullopt;
  }
}

template <>
[[nodiscard]] std::optional<double> get_env<double>(const char* name)
{
  auto val = get_env_string(name);
  if (!val || val->empty())
  {
    return std::nullopt;
  }
  try
  {
    return std::stod(*val);
  }
  catch (...)
  {
    std::cerr << "catalyst_async: Invalid double value for " << name << ": '" << *val << "'\n";
    return std::nullopt;
  }
}

template <typename T>
[[nodiscard]] T get_env(const char* name, T default_val)
{
  return get_env<T>(name).value_or(default_val);
}

[[nodiscard]] std::vector<int> parse_core_list(std::string s)
{
  std::vector<int> result;

  // Remove brackets and spaces
  s.erase(
    std::remove_if(s.begin(), s.end(), [](char c) { return c == '[' || c == ']' || c == ' '; }),
    s.end());

  if (s.empty())
  {
    return result;
  }

  // Parse comma-separated integers
  size_t start = 0;
  size_t end = 0;
  while ((end = s.find(',', start)) != std::string::npos || start < s.size())
  {
    if (end == std::string::npos)
    {
      end = s.size();
    }
    const std::string token = s.substr(start, end - start);
    if (token.empty())
    {
      std::cerr << "catalyst_async: Empty element in worker_cores list\n";
    }
    else
    {
      try
      {
        result.push_back(std::stoi(token));
      }
      catch (...)
      {
        std::cerr << "catalyst_async: Invalid core number '" << token << "' in worker_cores\n";
      }
    }
    start = end + 1;
    if (start > s.size())
    {
      break;
    }
  }
  return result;
}

// ============================================================================
// atexit handler for clean shutdown on abnormal termination
// ============================================================================

void async_cleanup_atexit()
{
  catalyst_async_abort();
}

// ============================================================================
// Thread affinity
// ============================================================================

[[nodiscard]] int detect_local_rank()
{
  for (const auto* env_var : kLocalRankEnvVars)
  {
    if (auto val = get_env<int>(env_var))
    {
      return *val;
    }
  }
  return 0;
}

[[nodiscard]] int get_num_cores()
{
#if CATALYST_ASYNC_USE_HWLOC
  hwloc_topology_t topo;
  hwloc_topology_init(&topo);
  hwloc_topology_load(topo);
  auto depth = hwloc_get_type_depth(topo, HWLOC_OBJ_CORE);
  int num_cores = (depth != HWLOC_TYPE_DEPTH_UNKNOWN)
    ? static_cast<int>(hwloc_get_nbobjs_by_depth(topo, depth))
    : 0;
  hwloc_topology_destroy(topo);
  return num_cores > 0 ? num_cores : static_cast<int>(std::thread::hardware_concurrency());
#else
  return static_cast<int>(std::thread::hardware_concurrency());
#endif
}

#ifdef __linux__
// Get the list of CPU cores allocated to this process by the job scheduler
// This respects SLURM/PBS/etc CPU allocations rather than assuming all system cores
[[nodiscard]] std::vector<int> get_allowed_cores()
{
  std::vector<int> cores;
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);

  if (sched_getaffinity(0, sizeof(cpu_set_t), &cpuset) == 0)
  {
    for (int i = 0; i < CPU_SETSIZE; i++)
    {
      if (CPU_ISSET(i, &cpuset))
      {
        cores.push_back(i);
      }
    }
  }
  return cores;
}
#endif

[[nodiscard]] std::optional<int> auto_detect_worker_core(int local_rank)
{
#ifdef __linux__
  // On Linux, query the scheduler-allocated cores
  auto allowed = get_allowed_cores();
  if (allowed.empty())
  {
    // Fall back to system core count if sched_getaffinity fails
    const int num_cores = get_num_cores();
    if (num_cores < 2)
    {
      return std::nullopt;
    }
    // Assume core 1 for worker (core 0 for simulation)
    return 1;
  }

  if (allowed.size() < 2)
  {
    // Need at least 2 cores: one for simulation, one for worker
    return std::nullopt;
  }

  // Pick the second core from our allowed set
  // (first core is for the simulation main thread)
  // Note: local_rank not used here because sched_getaffinity already returns
  // only the cores allocated to THIS rank by the job scheduler
  (void)local_rank;
  return allowed[1];
#else
  // Non-Linux: use the original logic based on total system cores
  int num_cores = get_num_cores();
  if (num_cores == 0)
  {
    return std::nullopt;
  }

  int ranks_per_node = 1;
  for (const auto* env_var : kLocalSizeEnvVars)
  {
    if (auto val = get_env<int>(env_var))
    {
      ranks_per_node = std::max(1, *val);
      break;
    }
  }

  int cores_per_rank = num_cores / ranks_per_node;
  if (cores_per_rank < 2)
  {
    return std::nullopt;
  }

  return local_rank * cores_per_rank + 1;
#endif
}

void apply_worker_affinity()
{
  if (g_ctx.config.affinity_mode == AsyncConfig::AffinityMode::None)
  {
    return;
  }

  const int local_rank = detect_local_rank();
  std::optional<int> worker_core;

  if (g_ctx.config.affinity_mode == AsyncConfig::AffinityMode::Manual)
  {
    if (local_rank >= 0 && static_cast<size_t>(local_rank) < g_ctx.config.worker_cores.size())
    {
      worker_core = g_ctx.config.worker_cores[local_rank];
    }
  }
  else
  {
    worker_core = auto_detect_worker_core(local_rank);
  }

  if (!worker_core)
  {
    if (g_ctx.config.verbose)
    {
      std::cerr << "catalyst_async: No worker core assigned, letting OS schedule\n";
    }
    return;
  }

#ifdef __linux__
  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(*worker_core, &mask);
  const int ret = pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask);
  if (ret == 0)
  {
    g_ctx.worker_pinned_core = *worker_core;
  }
  if (g_ctx.config.verbose)
  {
    std::cerr << "catalyst_async: Worker thread " << (ret == 0 ? "pinned to" : "failed to pin to")
              << " core " << *worker_core << "\n";
  }
#elif defined(_WIN32)
  // Use processor groups to support >64 cores
  WORD group = static_cast<WORD>(*worker_core / 64);
  KAFFINITY mask = 1ULL << (*worker_core % 64);
  GROUP_AFFINITY affinity = {};
  affinity.Group = group;
  affinity.Mask = mask;
  BOOL success = SetThreadGroupAffinity(GetCurrentThread(), &affinity, NULL);
  if (success)
  {
    g_ctx.worker_pinned_core = *worker_core;
  }
  if (g_ctx.config.verbose)
  {
    std::cerr << "catalyst_async: Worker thread " << (success ? "pinned to" : "failed to pin to")
              << " core " << *worker_core << " (group " << group << ")\n";
  }
#elif CATALYST_ASYNC_USE_HWLOC
  hwloc_topology_t topo;
  hwloc_topology_init(&topo);
  hwloc_topology_load(topo);
  hwloc_cpuset_t cpuset = hwloc_bitmap_alloc();
  hwloc_bitmap_set(cpuset, *worker_core);
  int ret = hwloc_set_cpubind(topo, cpuset, HWLOC_CPUBIND_THREAD);
  hwloc_bitmap_free(cpuset);
  hwloc_topology_destroy(topo);
  if (ret == 0)
  {
    g_ctx.worker_pinned_core = *worker_core;
  }
  if (g_ctx.config.verbose)
  {
    std::cerr << "catalyst_async: Worker thread " << (ret == 0 ? "pinned to" : "failed to pin to")
              << " core " << *worker_core << " (hwloc)\n";
  }
#else
  if (g_ctx.config.verbose)
  {
    std::cerr << "catalyst_async: Thread affinity not supported on this platform\n";
  }
#endif
}

// ============================================================================
// Worker thread
// ============================================================================

void worker_loop()
{
  apply_worker_affinity();

  while (true)
  {
    std::optional<WorkItem> item;

    {
      std::unique_lock lock(g_ctx.queue_mutex);
      g_ctx.worker_busy = false;
      g_ctx.queue_cv.notify_all(); // Wake anyone waiting on flush

      g_ctx.queue_cv.wait(lock, [] { return !g_ctx.work_queue.empty() || g_ctx.should_stop; });

      if (g_ctx.should_stop && g_ctx.work_queue.empty())
      {
        break;
      }

      if (!g_ctx.work_queue.empty())
      {
        item.emplace(std::move(g_ctx.work_queue.front()));
        g_ctx.work_queue.pop();
        g_ctx.worker_busy = true;

        auto wait_time = std::chrono::steady_clock::now() - item->enqueue_time;
        const double wait_sec = std::chrono::duration<double>(wait_time).count();
        g_ctx.stats.max_queue_wait = std::max(g_ctx.stats.max_queue_wait, wait_sec);
      }
    }

    // Copy to local to avoid races on g_ctx.impl_execute
    auto impl_execute = g_ctx.impl_execute;
    if (item && (item->type == WorkItem::Type::EXECUTE) && impl_execute)
    {
      auto start = std::chrono::steady_clock::now();

      const conduit_node* c_node = conduit::c_node(&item->data);

      // Lock impl_mutex: prevents concurrent access if main thread
      // calls catalyst_about() or catalyst_results() during execution
      enum catalyst_status status = catalyst_status_ok;
      bool had_exception = false;
      {
        const std::scoped_lock impl_lock(g_ctx.impl_mutex);
        try
        {
          status = (*impl_execute)(c_node);
        }
        catch (const std::exception& e)
        {
          std::cerr << "catalyst_async: Worker caught exception: " << e.what() << "\n";
          had_exception = true;
        }
        catch (...)
        {
          std::cerr << "catalyst_async: Worker caught unknown exception\n";
          had_exception = true;
        }
      }

      auto elapsed = std::chrono::steady_clock::now() - start;
      const double elapsed_sec = std::chrono::duration<double>(elapsed).count();
      g_ctx.stats.total_execute_time += elapsed_sec;
      g_ctx.stats.max_execute_time = std::max(g_ctx.stats.max_execute_time, elapsed_sec);

      if (had_exception || status != catalyst_status_ok)
      {
        g_ctx.stats.execute_errors++;
        if (!had_exception && g_ctx.config.verbose)
        {
          std::cerr << "catalyst_async: Implementation returned error status\n";
        }
      }

      if (elapsed_sec > g_ctx.config.slow_threshold_sec)
      {
        g_ctx.stats.timesteps_slow++;
        if (g_ctx.config.verbose)
        {
          std::cerr << "catalyst_async: Slow execute (" << elapsed_sec << "s > "
                    << g_ctx.config.slow_threshold_sec << "s threshold)\n";
        }
      }

      g_ctx.stats.timesteps_processed++;
    }
    else if (item &&
      (item->type == WorkItem::Type::INITIALIZE || item->type == WorkItem::Type::FINALIZE) &&
      item->impl_fn)
    {
      // Synchronous init/finalize items: run impl_fn on the worker thread
      // so that VTK (GC, Python interpreter) are always on the same thread
      // as impl_execute.  The caller holds a future and waits below.
      enum catalyst_status status = catalyst_status_ok;
      {
        const std::scoped_lock impl_lock(g_ctx.impl_mutex);
        const conduit_node* c_node = conduit::c_node(&item->data);
        try
        {
          status = (*item->impl_fn)(c_node);
        }
        catch (const std::exception& e)
        {
          std::cerr << "catalyst_async: Worker caught exception in init/finalize: " << e.what()
                    << "\n";
          status = catalyst_status_error_incomplete;
        }
        catch (...)
        {
          std::cerr << "catalyst_async: Worker caught unknown exception in init/finalize\n";
          status = catalyst_status_error_incomplete;
        }
      }
      if (item->sync_promise)
      {
        item->sync_promise->set_value(status);
      }
    }
  }
}

// ============================================================================
// Configuration loading
// ============================================================================

void load_config(const conduit_node* params)
{
  g_ctx.config.enabled = get_env<bool>("CATALYST_ASYNC_ENABLED", false);
  g_ctx.config.queue_depth = get_env<size_t>("CATALYST_ASYNC_QUEUE_DEPTH", kDefaultQueueDepth);
  g_ctx.config.verbose = get_env<bool>("CATALYST_ASYNC_VERBOSE", false);
  g_ctx.config.slow_threshold_sec = get_env<double>("CATALYST_ASYNC_SLOW_THRESHOLD", 10.0);
  g_ctx.config.flush_timeout_sec = get_env<double>("CATALYST_ASYNC_FLUSH_TIMEOUT", 300.0);

  if (auto mode = get_env_string("CATALYST_ASYNC_AFFINITY_MODE"))
  {
    if (*mode == "none")
    {
      g_ctx.config.affinity_mode = AsyncConfig::AffinityMode::None;
    }
    else if (*mode == "manual")
    {
      g_ctx.config.affinity_mode = AsyncConfig::AffinityMode::Manual;
    }
    else if (*mode == "auto")
    {
      g_ctx.config.affinity_mode = AsyncConfig::AffinityMode::Auto;
    }
    else
    {
      std::cerr << "catalyst_async: Unknown affinity mode '" << *mode
                << "', expected 'auto', 'manual', or 'none'\n";
    }
  }

  if (auto cores_str = get_env_string("CATALYST_ASYNC_WORKER_CORES"))
  {
    g_ctx.config.worker_cores = parse_core_list(*cores_str);
    if (!g_ctx.config.worker_cores.empty())
    {
      g_ctx.config.affinity_mode = AsyncConfig::AffinityMode::Manual;
    }
  }

  if (!params)
  {
    return;
  }

  const auto& node = *conduit::cpp_node(const_cast<conduit_node*>(params));

  if (node.has_path("catalyst/async/enabled"))
  {
    const auto& n = node["catalyst/async/enabled"];
    if (n.dtype().is_number())
    {
      g_ctx.config.enabled = n.to_int() != 0;
    }
    else
    {
      std::cerr << "catalyst_async: Invalid type for catalyst/async/enabled, expected integer\n";
    }
  }
  if (node.has_path("catalyst/async/queue_depth"))
  {
    const auto& n = node["catalyst/async/queue_depth"];
    if (n.dtype().is_number())
    {
      auto val = n.to_int64();
      if (val < 0)
      {
        std::cerr << "catalyst_async: Invalid queue_depth " << val << ", must be non-negative\n";
      }
      else
      {
        g_ctx.config.queue_depth = static_cast<size_t>(val);
      }
    }
    else
    {
      std::cerr
        << "catalyst_async: Invalid type for catalyst/async/queue_depth, expected integer\n";
    }
  }
  if (node.has_path("catalyst/async/verbose"))
  {
    const auto& n = node["catalyst/async/verbose"];
    if (n.dtype().is_number())
    {
      g_ctx.config.verbose = n.to_int() != 0;
    }
    else
    {
      std::cerr << "catalyst_async: Invalid type for catalyst/async/verbose, expected integer\n";
    }
  }
  if (node.has_path("catalyst/async/slow_threshold"))
  {
    const auto& n = node["catalyst/async/slow_threshold"];
    if (n.dtype().is_number())
    {
      g_ctx.config.slow_threshold_sec = n.to_double();
    }
    else
    {
      std::cerr
        << "catalyst_async: Invalid type for catalyst/async/slow_threshold, expected number\n";
    }
  }
  if (node.has_path("catalyst/async/flush_timeout"))
  {
    const auto& n = node["catalyst/async/flush_timeout"];
    if (n.dtype().is_number())
    {
      g_ctx.config.flush_timeout_sec = n.to_double();
    }
    else
    {
      std::cerr
        << "catalyst_async: Invalid type for catalyst/async/flush_timeout, expected number\n";
    }
  }

  if (node.has_path("catalyst/async/affinity/mode"))
  {
    const auto& n = node["catalyst/async/affinity/mode"];
    if (!n.dtype().is_string())
    {
      std::cerr
        << "catalyst_async: Invalid type for catalyst/async/affinity/mode, expected string\n";
    }
    else
    {
      auto mode = n.as_string();
      if (mode == "none")
      {
        g_ctx.config.affinity_mode = AsyncConfig::AffinityMode::None;
      }
      else if (mode == "manual")
      {
        g_ctx.config.affinity_mode = AsyncConfig::AffinityMode::Manual;
      }
      else if (mode == "auto")
      {
        g_ctx.config.affinity_mode = AsyncConfig::AffinityMode::Auto;
      }
      else
      {
        std::cerr << "catalyst_async: Unknown affinity mode '" << mode
                  << "', expected 'auto', 'manual', or 'none'\n";
      }
    }
  }

  if (node.has_path("catalyst/async/affinity/worker_cores"))
  {
    const auto& cores_node = node["catalyst/async/affinity/worker_cores"];
    g_ctx.config.worker_cores.clear();
    for (conduit::index_t i = 0; i < cores_node.number_of_children(); i++)
    {
      const auto& child = cores_node.child(i);
      if (child.dtype().is_number())
      {
        g_ctx.config.worker_cores.push_back(child.to_int());
      }
      else if (g_ctx.config.verbose)
      {
        std::cerr << "catalyst_async: Ignoring non-numeric worker_core at index " << i << "\n";
      }
    }
    if (!g_ctx.config.worker_cores.empty())
    {
      g_ctx.config.affinity_mode = AsyncConfig::AffinityMode::Manual;
    }
  }

#ifdef CATALYST_USE_MPI
  if (mpi_is_usable())
  {
    if (node.has_path("catalyst/mpi_comm") && node["catalyst/mpi_comm"].dtype().is_integer())
    {
      g_ctx.mpi_comm = MPI_Comm_f2c(node["catalyst/mpi_comm"].to_int());
    }
    else
    {
      g_ctx.mpi_comm = MPI_COMM_WORLD;
    }

    if (g_ctx.config.verbose)
    {
      int rank;
      MPI_Comm_rank(g_ctx.mpi_comm, &rank);
      if (rank == 0)
      {
        std::cerr << "catalyst_async: MPI enabled, using "
                  << (node.has_path("catalyst/mpi_comm") ? "provided" : "MPI_COMM_WORLD")
                  << " communicator for synchronized skip decisions\n";
      }
    }
  }
#endif
}

void print_stats()
{
  if (!g_ctx.config.verbose)
  {
    return;
  }

#ifdef CATALYST_USE_MPI
  int rank = 0;
  if (g_ctx.mpi_comm != MPI_COMM_NULL)
  {
    MPI_Comm_rank(g_ctx.mpi_comm, &rank);
  }
#else
  const int rank = 0;
#endif

  if (rank == 0)
  {
    auto n = g_ctx.stats.timesteps_processed.load();
    auto slow_threshold = static_cast<int>(g_ctx.config.slow_threshold_sec);
    std::cerr << "\n======= CATALYST ASYNC STATISTICS =======\n"
              << "Mode: Asynchronous\n"
              << "Queue depth limit: " << g_ctx.config.queue_depth << "\n"
              << "Timesteps processed: " << n << "\n"
              << "Timesteps skipped: " << g_ctx.stats.timesteps_skipped.load() << "\n"
              << "Execute errors: " << g_ctx.stats.execute_errors.load() << "\n"
              << "Slow executes (>" << slow_threshold << "s): " << g_ctx.stats.timesteps_slow.load()
              << "\n"
              << "Max queue depth seen: " << g_ctx.stats.max_queue_depth_seen << "\n"
              << std::fixed << "Total copy time: " << g_ctx.stats.total_copy_time << " s\n"
              << "Total execute time: " << g_ctx.stats.total_execute_time << " s\n"
              << "Max execute time: " << g_ctx.stats.max_execute_time << " s\n";
    if (n > 0)
    {
      std::cerr << "Avg copy per output: "
                << (g_ctx.stats.total_copy_time / static_cast<double>(n) * 1000) << " ms\n"
                << "Avg execute per output: "
                << (g_ctx.stats.total_execute_time / static_cast<double>(n) * 1000) << " ms\n";
    }
    std::cerr << "Max queue wait: " << g_ctx.stats.max_queue_wait << " s\n"
              << "=========================================\n"
              << std::defaultfloat << '\n';
  }
}

} // anonymous namespace

// ============================================================================
// Public C API
// ============================================================================

extern "C"
{

  enum catalyst_status catalyst_async_initialize(
    const conduit_node* params, enum catalyst_status (*impl_initialize)(const conduit_node*))
  {
    if (g_ctx.initialized)
    {
      if (g_ctx.config.verbose)
      {
        std::cerr << "catalyst_async: Already initialized, ignoring request\n";
      }
      return catalyst_status_ok;
    }

    // Guard against concurrent initialization
    bool expected = false;
    if (!g_ctx.initializing.compare_exchange_strong(expected, true))
    {
      return catalyst_status_ok;
    }

    load_config(params);

    if (!g_ctx.config.enabled)
    {
      if (g_ctx.config.verbose)
      {
        std::cerr << "catalyst_async: Async mode not enabled\n";
      }
      g_ctx.initializing = false;
      // Run impl_initialize synchronously on the calling thread (unchanged behavior)
      return (*impl_initialize)(params);
    }

    // Reset state
    g_ctx.should_stop = false;
    g_ctx.worker_busy = false;
    g_ctx.worker_pinned_core = -1;
    g_ctx.stats.timesteps_processed = 0;
    g_ctx.stats.timesteps_skipped = 0;
    g_ctx.stats.execute_errors = 0;
    g_ctx.stats.timesteps_slow = 0;
    g_ctx.stats.max_queue_depth_seen = 0;
    g_ctx.stats.total_copy_time = 0.0;
    g_ctx.stats.total_execute_time = 0.0;
    g_ctx.stats.max_execute_time = 0.0;
    g_ctx.stats.max_queue_wait = 0.0;
    g_ctx.work_queue = {};
    g_ctx.gpu_runtime = {}; // Reset GPU runtime state

    // Detect GPU runtime (dlopen-based, no build dependency)
    init_gpu_runtime();

    // Start worker thread
    g_ctx.worker_thread = std::thread(worker_loop);

    // Register cleanup for abnormal termination (Ctrl+C, etc.)
    // Only register once across init/finalize/init cycles
    static bool atexit_registered = false;
    if (!atexit_registered)
    {
      std::atexit(async_cleanup_atexit);
      atexit_registered = true;
    }

    // Run impl_initialize on the worker thread and wait for completion.
    //
    // Fixes issue #50: Python's interpreter state is thread-local -- bound to
    // whichever thread calls impl_initialize. Previously impl_finalize ran on
    // the simulation main thread and crashed calling back into Python (SIGSEGV).
    // Running all three impl calls on the same worker thread keeps Python's
    // thread state consistent.
    //
    // vtkGarbageCollector records the "main thread" at dlopen() time, before
    // this runs, so the debug-mode GC assertion is not resolved here. In
    // practice this is benign: vtkAlgorithm/vtkExecutive cycles are created
    // once at pipeline construction and held for the session lifetime -- a
    // fixed overhead independent of timestep count. A proper fix requires
    // vtkGarbageCollector::SetMainThread() as a separate VTK enhancement.
    //
    // catalyst_initialize() remains synchronous from the simulation's perspective.
    {
      auto promise = std::make_shared<std::promise<enum catalyst_status>>();
      auto future = promise->get_future();

      WorkItem item;
      item.type = WorkItem::Type::INITIALIZE;
      item.impl_fn = impl_initialize;
      item.sync_promise = promise;
      if (params)
      {
        conduit::cpp_node(const_cast<conduit_node*>(params))->compact_to(item.data);
      }

      {
        const std::scoped_lock lock(g_ctx.queue_mutex);
        g_ctx.work_queue.push(std::move(item));
      }
      g_ctx.queue_cv.notify_one();

      const enum catalyst_status init_status = future.get(); // blocks until worker runs it
      if (init_status != catalyst_status_ok)
      {
        // impl_initialize failed — tear down the worker cleanly
        {
          const std::scoped_lock lock(g_ctx.queue_mutex);
          g_ctx.should_stop = true;
        }
        g_ctx.queue_cv.notify_all();
        if (g_ctx.worker_thread.joinable())
        {
          g_ctx.worker_thread.join();
        }
        g_ctx.initializing = false;
        return init_status;
      }
    }

    g_ctx.initialized = true;
    g_ctx.initializing = false;

    if (g_ctx.config.verbose)
    {
      std::cerr << "catalyst_async: Initialized (queue_depth=" << g_ctx.config.queue_depth
                << ", impl_initialize ran on worker thread)\n";
    }

    return catalyst_status_ok;
  }

  int catalyst_async_is_enabled(void)
  {
    return (g_ctx.config.enabled && g_ctx.initialized) ? 1 : 0;
  }

  enum catalyst_status catalyst_async_execute(
    const conduit_node* params, enum catalyst_status (*impl_execute)(const conduit_node*))
  {
    if (!g_ctx.config.enabled || !g_ctx.initialized)
    {
      return (*impl_execute)(params); // Synchronous fallback
    }

    // Store impl for worker thread
    if (g_ctx.impl_execute != nullptr && g_ctx.impl_execute != impl_execute)
    {
      std::cerr << "catalyst_async: WARNING: impl_execute function pointer changed unexpectedly\n";
    }
    g_ctx.impl_execute = impl_execute;

    // ----------------------------------------------------------------
    // Determine if we can enqueue (queue has space)
    // ----------------------------------------------------------------
    int local_can_enqueue;
    {
      const std::scoped_lock lock(g_ctx.queue_mutex);
      local_can_enqueue = (g_ctx.work_queue.size() < g_ctx.config.queue_depth) ? 1 : 0;
    }

#ifdef CATALYST_USE_MPI
    // CRITICAL: All ranks must make the same enqueue/skip decision,
    // because the Catalyst implementation (e.g., ParaView) may call
    // MPI collectives during execute. If ranks disagree, we deadlock.
    //
    // MPI_MIN: if ANY rank's queue is full, ALL ranks skip.
    int global_can_enqueue = local_can_enqueue;
    if (g_ctx.mpi_comm != MPI_COMM_NULL)
    {
      MPI_Allreduce(&local_can_enqueue, &global_can_enqueue, 1, MPI_INT, MPI_MIN, g_ctx.mpi_comm);
    }
#else
    const int global_can_enqueue = local_can_enqueue;
#endif

    if (!global_can_enqueue)
    {
      if (g_ctx.config.verbose)
      {
        std::cerr << "catalyst_async: All ranks skipping (queue full)\n";
      }
      g_ctx.stats.timesteps_skipped++;
      return catalyst_status_ok;
    }

    // ----------------------------------------------------------------
    // Deep copy the data (on main thread, before returning)
    // ----------------------------------------------------------------
    auto start_copy = std::chrono::steady_clock::now();

    WorkItem item;
    item.enqueue_time = start_copy;

    // Need non-const access to potentially replace GPU pointers with CPU copies
    auto& src = *conduit::cpp_node(const_cast<conduit_node*>(params));

    // If GPU runtime available, copy any device pointers to CPU before compact_to
    copy_gpu_arrays_to_cpu(src);

    src.compact_to(item.data);

    // Clear temporary GPU copy buffers now that compact_to is done
    clear_gpu_copy_buffers();

    auto copy_elapsed = std::chrono::steady_clock::now() - start_copy;
    g_ctx.stats.total_copy_time += std::chrono::duration<double>(copy_elapsed).count();

    // ----------------------------------------------------------------
    // Enqueue
    // ----------------------------------------------------------------
    {
      const std::scoped_lock lock(g_ctx.queue_mutex);

      auto new_depth = g_ctx.work_queue.size() + 1;
      g_ctx.stats.max_queue_depth_seen = std::max(g_ctx.stats.max_queue_depth_seen, new_depth);

      g_ctx.work_queue.push(std::move(item));
    }

    g_ctx.queue_cv.notify_one();

    // Check for flush request in params
    // catalyst/async/flush = 1 means wait for all work to complete before returning
    if (src.has_path("catalyst/async/flush"))
    {
      const auto& flush_node = src["catalyst/async/flush"];
      if (flush_node.dtype().is_integer() && flush_node.to_int64() != 0)
      {
        catalyst_async_flush();
      }
    }

    return catalyst_status_ok;
  }

  void catalyst_async_flush(void)
  {
    if (!g_ctx.config.enabled || !g_ctx.initialized)
    {
      return;
    }

    std::unique_lock lock(g_ctx.queue_mutex);

    if (g_ctx.config.flush_timeout_sec > 0)
    {
      auto timeout = std::chrono::duration<double>(g_ctx.config.flush_timeout_sec);
      const bool completed = g_ctx.queue_cv.wait_for(
        lock, timeout, [] { return g_ctx.work_queue.empty() && !g_ctx.worker_busy; });

      if (!completed)
      {
        std::cerr << "catalyst_async: WARNING: flush timed out after "
                  << g_ctx.config.flush_timeout_sec << "s, worker may be hung\n";
        std::cerr << "catalyst_async:   Queue depth: " << g_ctx.work_queue.size()
                  << ", worker_busy: " << (g_ctx.worker_busy ? "yes" : "no") << "\n";
      }
    }
    else
    {
      // No timeout - wait indefinitely
      g_ctx.queue_cv.wait(lock, [] { return g_ctx.work_queue.empty() && !g_ctx.worker_busy; });
    }
  }

  int catalyst_async_has_pending_work(void)
  {
    if (!g_ctx.config.enabled || !g_ctx.initialized)
    {
      return 0;
    }

    const std::scoped_lock lock(g_ctx.queue_mutex);
    return (!g_ctx.work_queue.empty() || g_ctx.worker_busy) ? 1 : 0;
  }

  size_t catalyst_async_queue_depth(void)
  {
    if (!g_ctx.config.enabled || !g_ctx.initialized)
    {
      return 0;
    }

    const std::scoped_lock lock(g_ctx.queue_mutex);
    return g_ctx.work_queue.size();
  }

  enum catalyst_status catalyst_async_finalize(
    const conduit_node* params, enum catalyst_status (*impl_finalize)(const conduit_node*))
  {
    if (!g_ctx.initialized)
    {
      // Async was not enabled; caller is responsible for running impl_finalize
      // on the calling thread (unchanged synchronous behavior).
      return (*impl_finalize)(params);
    }

    // Flush all pending execute items first.
    catalyst_async_flush();

    // Enqueue FINALIZE on the worker thread and wait for completion.
    // impl_finalize must run on the same thread as impl_initialize and
    // impl_execute to satisfy Python interpreter thread-state requirements
    // (PyUnicode_FromString and friends assert the calling thread owns the
    // interpreter state, which was set up on the worker in impl_initialize).
    enum catalyst_status fin_status = catalyst_status_ok;
    {
      auto promise = std::make_shared<std::promise<enum catalyst_status>>();
      auto future = promise->get_future();

      WorkItem item;
      item.type = WorkItem::Type::FINALIZE;
      item.impl_fn = impl_finalize;
      item.sync_promise = promise;
      if (params)
      {
        conduit::cpp_node(const_cast<conduit_node*>(params))->compact_to(item.data);
      }

      {
        const std::scoped_lock lock(g_ctx.queue_mutex);
        g_ctx.work_queue.push(std::move(item));
      }
      g_ctx.queue_cv.notify_one();

      fin_status = future.get(); // blocks until worker runs it
    }

    // Now safe to stop the worker.
    {
      const std::scoped_lock lock(g_ctx.queue_mutex);
      g_ctx.should_stop = true;
    }
    g_ctx.queue_cv.notify_all();

    if (g_ctx.worker_thread.joinable())
    {
      g_ctx.worker_thread.join();
    }

    print_stats();

    g_ctx.initialized = false;
    g_ctx.impl_execute = nullptr;
#ifdef CATALYST_USE_MPI
    g_ctx.mpi_comm = MPI_COMM_NULL;
#endif

    return fin_status;
  }

  void catalyst_async_abort(void)
  {
    if (!g_ctx.initialized)
    {
      return;
    }

    // Clear the queue and stop immediately - don't process pending work
    {
      const std::scoped_lock lock(g_ctx.queue_mutex);
      while (!g_ctx.work_queue.empty())
      {
        g_ctx.work_queue.pop();
      }
      g_ctx.should_stop = true;
    }
    g_ctx.queue_cv.notify_all();

    if (g_ctx.worker_thread.joinable())
    {
      g_ctx.worker_thread.join();
    }

    // No stats printed on abort

    g_ctx.initialized = false;
    g_ctx.impl_execute = nullptr;
#ifdef CATALYST_USE_MPI
    g_ctx.mpi_comm = MPI_COMM_NULL;
#endif
  }

  void catalyst_async_get_stats(conduit_node* stats)
  {
    if (!stats)
    {
      return;
    }

    auto& node = *conduit::cpp_node(stats);
    node["stats/timesteps_processed"] =
      static_cast<conduit::int64>(g_ctx.stats.timesteps_processed.load());
    node["stats/timesteps_skipped"] =
      static_cast<conduit::int64>(g_ctx.stats.timesteps_skipped.load());
    node["stats/execute_errors"] = static_cast<conduit::int64>(g_ctx.stats.execute_errors.load());
    node["stats/timesteps_slow"] = static_cast<conduit::int64>(g_ctx.stats.timesteps_slow.load());
    node["stats/max_queue_depth_seen"] =
      static_cast<conduit::int64>(g_ctx.stats.max_queue_depth_seen);
    node["stats/total_copy_time_sec"] = g_ctx.stats.total_copy_time;
    node["stats/total_execute_time_sec"] = g_ctx.stats.total_execute_time;
    node["stats/max_execute_time_sec"] = g_ctx.stats.max_execute_time;
    node["stats/max_queue_wait_sec"] = g_ctx.stats.max_queue_wait;
  }

  void catalyst_async_get_info(conduit_node* info)
  {
    if (!info)
    {
      return;
    }

    auto& node = *conduit::cpp_node(info);

    // Configuration
    node["config/enabled"] = g_ctx.config.enabled ? 1 : 0;
    node["config/queue_depth"] = static_cast<conduit::int64>(g_ctx.config.queue_depth);

    const char* mode_str = nullptr;
    switch (g_ctx.config.affinity_mode)
    {
      case AsyncConfig::AffinityMode::Auto:
        mode_str = "auto";
        break;
      case AsyncConfig::AffinityMode::Manual:
        mode_str = "manual";
        break;
      case AsyncConfig::AffinityMode::None:
        mode_str = "none";
        break;
    }
    node["config/affinity_mode"] = mode_str;

#if CATALYST_ASYNC_USE_HWLOC
    node["config/hwloc_available"] = 1;
#else
    node["config/hwloc_available"] = 0;
#endif

    // GPU runtime status
    const char* gpu_backend_str = nullptr;
    switch (g_ctx.gpu_runtime.backend)
    {
      case GpuRuntime::Backend::CUDA:
        gpu_backend_str = "cuda";
        break;
      case GpuRuntime::Backend::HIP:
        gpu_backend_str = "hip";
        break;
      case GpuRuntime::Backend::None:
        gpu_backend_str = "none";
        break;
    }
    node["config/gpu_runtime"] = gpu_backend_str;

    // Worker status
    node["worker/pinned_core"] = g_ctx.worker_pinned_core.load();
  }

  void catalyst_async_impl_lock(void)
  {
    if (g_ctx.config.enabled && g_ctx.initialized)
    {
      g_ctx.impl_mutex.lock();
    }
  }

  void catalyst_async_impl_unlock(void)
  {
    if (g_ctx.config.enabled && g_ctx.initialized)
    {
      g_ctx.impl_mutex.unlock();
    }
  }

  void catalyst_async_populate_about(conduit_node* params)
  {
    if (!params)
    {
      return;
    }

    auto& node = *conduit::cpp_node(params);

    // Report runtime state: enabled AND initialized
    // This matches the behavior of the old catalyst_is_async_enabled()
    node["catalyst/async/enabled"] = (g_ctx.config.enabled && g_ctx.initialized) ? 1 : 0;
    node["catalyst/async/queue_depth"] = static_cast<conduit::int64>(g_ctx.config.queue_depth);

    const char* mode_str = nullptr;
    switch (g_ctx.config.affinity_mode)
    {
      case AsyncConfig::AffinityMode::Auto:
        mode_str = "auto";
        break;
      case AsyncConfig::AffinityMode::Manual:
        mode_str = "manual";
        break;
      case AsyncConfig::AffinityMode::None:
        mode_str = "none";
        break;
    }
    node["catalyst/async/affinity_mode"] = mode_str;

#if CATALYST_ASYNC_USE_HWLOC
    node["catalyst/async/hwloc_available"] = 1;
#else
    node["catalyst/async/hwloc_available"] = 0;
#endif

    // GPU runtime status
    const char* gpu_backend_str = nullptr;
    switch (g_ctx.gpu_runtime.backend)
    {
      case GpuRuntime::Backend::CUDA:
        gpu_backend_str = "cuda";
        break;
      case GpuRuntime::Backend::HIP:
        gpu_backend_str = "hip";
        break;
      case GpuRuntime::Backend::None:
        gpu_backend_str = "none";
        break;
    }
    node["catalyst/async/gpu_runtime"] = gpu_backend_str;

    node["catalyst/async/worker_pinned_core"] = g_ctx.worker_pinned_core.load();

    // Include runtime stats
    node["catalyst/async/stats/timesteps_processed"] =
      static_cast<conduit::int64>(g_ctx.stats.timesteps_processed.load());
    node["catalyst/async/stats/timesteps_skipped"] =
      static_cast<conduit::int64>(g_ctx.stats.timesteps_skipped.load());
    node["catalyst/async/stats/execute_errors"] =
      static_cast<conduit::int64>(g_ctx.stats.execute_errors.load());
  }

} // extern "C"
