/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

#ifndef catalyst_async_h
#define catalyst_async_h

#include "catalyst_api.h"
#include <conduit.h>

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * Configuration for async execution mode.
   *
   * Async mode allows catalyst_execute() to return immediately while
   * the actual work is performed on a background thread. This enables
   * overlapping visualization with simulation, particularly useful
   * for GPU simulations where CPU cores are idle during compute kernels.
   */

  /**
   * Initialize async execution support.
   *
   * Configuration is read from:
   * 1. The params node (takes precedence):
   *    - params["catalyst/async/enabled"] = true/false
   *    - params["catalyst/async/queue_depth"] = int (default: 2)
   *    - params["catalyst/async/affinity/mode"] = "auto" | "manual" | "none"
   *    - params["catalyst/async/affinity/worker_cores"] = [1, 2, 3, 4]
   *      (one core per local MPI rank; e.g., rank 0 uses core 1, rank 1 uses core 2)
   *    - params["catalyst/async/verbose"] = true/false
   *
   * 2. Environment variables (if not in params):
   *    - CATALYST_ASYNC_ENABLED=1
   *    - CATALYST_ASYNC_QUEUE_DEPTH=2
   *    - CATALYST_ASYNC_AFFINITY_MODE=auto|manual|none
   *    - CATALYST_ASYNC_WORKER_CORES=1,2,3,4
   *    - CATALYST_ASYNC_VERBOSE=1
   *
   * @param params Configuration node (may be NULL)
   * @return catalyst_status_ok on success
   */
  enum catalyst_status catalyst_async_initialize(const conduit_node* params);

  /**
   * Check if async mode is currently enabled.
   *
   * @return Non-zero if async mode is active
   */
  int catalyst_async_is_enabled(void);

  /**
   * Execute asynchronously.
   *
   * If async is enabled, this will:
   * 1. Deep-copy the params node (converting external refs to owned data)
   * 2. Queue the work for the background thread
   * 3. Return immediately
   *
   * If the queue is full, behavior depends on configuration:
   * - drop_newest (default): Skip this timestep, return immediately
   * - drop_oldest: Remove oldest item from queue, add this one
   * - block: Wait for space (defeats purpose of async)
   *
   * @param params The execute parameters (will be copied)
   * @param impl_execute The implementation's execute function pointer
   * @return catalyst_status_ok on success (or if skipped)
   */
  enum catalyst_status catalyst_async_execute(
    const conduit_node* params, enum catalyst_status (*impl_execute)(const conduit_node*));

  /**
   * Wait for all pending async work to complete.
   *
   * This should be called before finalize to ensure all queued
   * work is processed. Also useful if you need results from
   * a previous execute before continuing.
   */
  void catalyst_async_flush(void);

  /**
   * Check if there is pending async work.
   *
   * @return Non-zero if work is queued or being processed
   */
  int catalyst_async_has_pending_work(void);

  /**
   * Get the current queue depth.
   *
   * @return Number of items currently in the queue
   */
  size_t catalyst_async_queue_depth(void);

  /**
   * Finalize async support.
   *
   * This will:
   * 1. Flush any pending work
   * 2. Stop the worker thread
   * 3. Print statistics (if verbose)
   *
   * @param params Finalize parameters (may be NULL)
   */
  void catalyst_async_finalize(const conduit_node* params);

  /**
   * Abort async execution immediately without draining the queue.
   *
   * Unlike catalyst_async_finalize(), this discards any pending work
   * and stops the worker thread immediately. Used by atexit handler
   * for clean shutdown on abnormal termination.
   */
  void catalyst_async_abort(void);

  /**
   * Get async execution statistics.
   *
   * Fills the provided node with:
   * - stats/timesteps_processed
   * - stats/timesteps_skipped
   * - stats/max_queue_depth_seen
   * - stats/total_copy_time_sec
   * - stats/total_execute_time_sec
   * - stats/max_queue_wait_sec
   *
   * @param stats Node to fill with statistics
   */
  void catalyst_async_get_stats(conduit_node* stats);

  /**
   * Get async configuration and status information.
   *
   * Fills the provided node with:
   * - config/enabled (bool)
   * - config/queue_depth (int)
   * - config/affinity_mode (string: "auto", "manual", "none")
   * - config/hwloc_available (bool: whether hwloc support was compiled in)
   * - worker/pinned_core (int: the core the worker is pinned to, -1 if not pinned)
   *
   * @param info Node to fill with information
   */
  void catalyst_async_get_info(conduit_node* info);

  /**
   * Lock/unlock the implementation mutex.
   *
   * When async mode is active, the worker thread may be calling
   * impl->execute(). These functions allow catalyst_about() and
   * catalyst_results() to safely access the implementation without
   * data races.
   *
   * If async is not enabled, these are no-ops.
   */
  void catalyst_async_impl_lock(void);
  void catalyst_async_impl_unlock(void);

  /**
   * Add async-related information to a catalyst_about() response.
   *
   * Populates the node with:
   * - catalyst/async/enabled (int)
   * - catalyst/async/queue_depth (int)
   * - catalyst/async/affinity_mode (string)
   * - catalyst/async/hwloc_available (int)
   * - catalyst/async/worker_pinned_core (int, -1 if not pinned)
   *
   * @param params The about node to populate
   */
  void catalyst_async_populate_about(conduit_node* params);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // catalyst_async_h
