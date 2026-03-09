Asynchronous Execution
**********************

Catalyst supports an optional asynchronous execution mode that allows
``catalyst_execute()`` to return immediately while the actual in-situ work
runs on a background thread. This is particularly useful for GPU-based
simulations where CPU cores sit idle during compute kernels.


Motivation
==========

In a typical in situ workflow, ``catalyst_execute()`` is synchronous: the
simulation blocks while the Catalyst implementation processes data, directly
extending wall-clock time. However, many modern HPC simulations run primarily
on GPUs. During GPU compute kernels, the CPU cores that launched those kernels
are largely idle, waiting for results. Async mode exploits these idle CPU
cores to run Catalyst processing overlapped with GPU computation.

.. code-block:: text

   Synchronous:
   ┌──────────┐┌────────┐┌──────────┐┌────────┐
   │ Simulate ││Catalyst││ Simulate ││Catalyst│  ← CPU blocked during Catalyst
   └──────────┘└────────┘└──────────┘└────────┘

   Asynchronous:
   ┌──────────┐┌──────────┐┌──────────┐
   │ Simulate ││ Simulate ││ Simulate │         ← GPU compute (CPU idle)
   └──────────┘└──────────┘└──────────┘
              ┌────────┐  ┌────────┐            ← Worker thread uses idle CPU
              │Catalyst│  │Catalyst│
              └────────┘  └────────┘


Design Principles
=================

**Transparent to adaptors.** Existing adaptor code calls
``catalyst_execute()`` exactly as before. The async machinery is entirely
within ``libcatalyst``; no adaptor or implementation changes are required.

**Transparent to implementations.** ParaView Catalyst, Ascent, or any custom
implementation receives ``impl->execute()`` calls as usual, unaware that
they are running on a worker thread.

**Opt-in.** Async mode is disabled by default. It is enabled via environment
variables or the ``params`` node passed to ``catalyst_initialize()``.

**MPI-safe.** When ``CATALYST_USE_MPI`` is enabled, all ranks make
synchronized skip decisions to prevent collective deadlocks.


Data Flow
=========

When async mode is enabled, ``catalyst_execute()`` follows this sequence:

.. code-block:: text

   catalyst_execute(params)
     │
     ├─ 1. Check queue capacity (local)
     │
     ├─ 2. MPI_Allreduce to agree on enqueue/skip  [MPI only]
     │      (if ANY rank is full, ALL ranks skip)
     │
     ├─ 3. GPU → CPU copy (if GPU runtime detected)
     │      • Uses dlopen'd cudaMemcpy/hipMemcpy
     │      • Only for external arrays with device pointers
     │
     ├─ 4. Deep copy: src.compact_to(copy)
     │      • Converts external pointers to owned data
     │      • Simulation can safely modify its buffers after this
     │
     ├─ 5. Enqueue work item
     │
     └─ 6. Return catalyst_status_ok immediately
              │
              │  (meanwhile, on worker thread)
              │
              ├─ Dequeue work item
              ├─ Lock impl mutex
              ├─ impl->execute(copied_data)
              ├─ Unlock impl mutex
              └─ Free copied data

The deep copy in step 4 is the key to correctness: after
``compact_to()`` completes, the copied data is fully independent of
the simulation's memory. The simulation can immediately advance to the
next timestep without waiting for the Catalyst implementation to finish.

.. note::

   **GPU Memory Handling**: The ``compact_to()`` operation requires CPU-accessible
   memory. The async layer automatically handles GPU device pointers:

   - **Automatic detection**: At runtime, Catalyst uses ``dlopen`` to check if
     CUDA (``libcudart.so``) or HIP (``libamdhip64.so``) is loaded. If found,
     it caches function pointers for ``cudaPointerGetAttributes``/``cudaMemcpy``
     (or the HIP equivalents).

   - **Transparent copy**: Before ``compact_to()``, the async layer walks the
     Conduit node tree. For any external array with a GPU device pointer, it
     performs a device-to-host copy to a temporary CPU buffer and updates the
     node to reference that buffer.

   - **No build-time dependency**: This uses ``dlopen``/``dlsym`` at runtime,
     so Catalyst has no compile-time CUDA/HIP dependency. The detection uses
     ``RTLD_NOLOAD`` to only find already-loaded libraries (those loaded by
     the simulation), avoiding CUDA version mismatches.

   - **Adaptor compatibility**: If the adaptor already copies GPU→CPU before
     calling ``catalyst_execute()`` (like the NekRS/ASCENT integration), the
     async layer sees CPU pointers and skips the detection/copy step.

   - **Graceful fallback**: If no GPU runtime is detected, the async layer
     assumes all pointers are CPU-accessible. Enable verbose mode
     (``CATALYST_ASYNC_VERBOSE=1``) to see which runtime was detected.


MPI Synchronization
===================

Catalyst implementations such as ParaView Catalyst use MPI collectives
internally (for parallel rendering, ghost exchange, etc.). If different ranks
make different enqueue/skip decisions, ranks will enter collectives out of
sync, causing deadlocks.

To prevent this, when ``CATALYST_USE_MPI`` is enabled, the async layer
performs a lightweight ``MPI_Allreduce`` on each execute call:

.. code-block:: c

   int local_can_enqueue = (queue_size < max_depth) ? 1 : 0;
   int global_can_enqueue;
   MPI_Allreduce(&local_can_enqueue, &global_can_enqueue,
                 1, MPI_INT, MPI_MIN, comm);

This ensures that if **any** rank's queue is full, **all** ranks skip that
timestep. The cost is a single integer reduction, which is negligible compared
to the data processing work.

When ``CATALYST_USE_MPI`` is not enabled, the allreduce compiles out entirely.
Single-rank execution has no collective ordering concern.

The MPI communicator is obtained from ``params["catalyst/mpi_comm"]`` during
``catalyst_initialize()``. If not provided, ``MPI_COMM_WORLD`` is used as
a fallback (which is correct for the vast majority of HPC codes).


Thread Safety
=============

The async layer introduces a worker thread that calls ``impl->execute()``.
Since implementations like ParaView Catalyst use VTK, which is not thread-safe
for concurrent object access, additional locking is required.

A global ``impl_mutex`` is used to ensure exclusive access:

- The **worker thread** holds ``impl_mutex`` during ``impl->execute()``.
- The **main thread** acquires ``impl_mutex`` before calling ``impl->about()``
  or ``impl->results()``.

In practice, ``about()`` and ``results()`` are rarely called during the
simulation loop, so contention is minimal. The ``catalyst_execute()`` path on
the main thread does **not** acquire the mutex; it only copies data and
enqueues, which involves only the queue mutex.


Thread Affinity
===============

For optimal performance, the worker thread should be pinned to a specific
CPU core, ideally on the same NUMA domain as the simulation rank it serves
but on a core not used by the simulation.

The async layer supports three affinity modes:

**Auto** (default)
   Detects the local MPI rank, total cores, and ranks per node using
   environment variables. Assigns the worker to the second core in each
   rank's core range. For example, with 64 cores and 4 ranks per node,
   rank 0's worker goes to core 1, rank 1's to core 17, etc.

   On Linux, ``pthread_setaffinity_np`` is used. On Windows,
   ``SetThreadAffinityMask`` is used. If ``hwloc`` is available
   (``CATALYST_ASYNC_USE_HWLOC=ON``), ``hwloc_set_cpubind`` is used instead,
   which provides cross-platform support including macOS.

**Manual**
   The user specifies worker cores explicitly per local rank:

   .. code-block:: bash

      export CATALYST_ASYNC_WORKER_CORES="1,2,3,4"

   Local rank 0 gets core 1, rank 1 gets core 17, and so on.

**None**
   No affinity is applied. The OS scheduler places the worker thread freely.
   This is appropriate for testing or when the simulation already manages
   all CPU affinity.



Configuration
=============

Async mode is configured through environment variables or the ``params`` node
passed to ``catalyst_initialize()``. The ``params`` node takes precedence when
both are specified.

.. list-table::
   :header-rows: 1
   :widths: 35 20 15 30

   * - Setting
     - Environment Variable
     - Default
     - Description
   * - Enable async
     - ``CATALYST_ASYNC_ENABLED``
     - ``0``
     - Set to ``1`` to enable async execution
   * - Queue depth
     - ``CATALYST_ASYNC_QUEUE_DEPTH``
     - ``2``
     - Maximum pending work items per rank
   * - Affinity mode
     - ``CATALYST_ASYNC_AFFINITY_MODE``
     - ``auto``
     - ``auto``, ``manual``, or ``none``
   * - Worker cores
     - ``CATALYST_ASYNC_WORKER_CORES``
     - (auto)
     - Comma-separated core IDs per local rank
   * - Verbose output
     - ``CATALYST_ASYNC_VERBOSE``
     - ``0``
     - Print debug info and statistics
   * - Slow threshold
     - ``CATALYST_ASYNC_SLOW_THRESHOLD``
     - ``10.0``
     - Log warning if execute exceeds this (seconds)
   * - Flush timeout
     - ``CATALYST_ASYNC_FLUSH_TIMEOUT``
     - ``300.0``
     - Timeout for flush (seconds), 0 = no timeout

Equivalent ``params`` paths:

.. code-block:: python

   params["catalyst/async/enabled"] = 1
   params["catalyst/async/queue_depth"] = 2
   params["catalyst/async/affinity/mode"] = "auto"
   params["catalyst/async/affinity/worker_cores"] = [1, 2, 3, 4]
   params["catalyst/async/verbose"] = 1
   params["catalyst/async/slow_threshold"] = 10.0
   params["catalyst/async/flush_timeout"] = 300.0


Public API
==========

The async feature uses the existing Catalyst API with no new functions,
preserving ABI stability. Control is via params:

**Query async status** via ``catalyst_about()``:

.. code-block:: c

   conduit_node* about = conduit_node_create();
   catalyst_about(about);
   int enabled = conduit_node_fetch_path_as_int64(about, "catalyst/async/enabled");
   conduit_node_destroy(about);

**Flush pending work** via params to ``catalyst_execute()``:

.. code-block:: c

   conduit_node* params = conduit_node_create();
   conduit_node_set_path_int64(params, "catalyst/async/flush", 1);
   catalyst_execute(params);  // Waits for all pending work to complete
   conduit_node_destroy(params);

The flush is automatically performed by ``catalyst_finalize()``, so most
simulations don't need to call it explicitly. Use flush if you need
synchronization points, e.g., before calling ``catalyst_results()``.

``catalyst_about()`` includes async configuration and status:

.. code-block:: text

   catalyst/async/enabled           (int: 1 if enabled, 0 otherwise)
   catalyst/async/queue_depth       (int: configured queue depth)
   catalyst/async/affinity_mode     (string: "auto", "manual", or "none")
   catalyst/async/hwloc_available   (int: 1 if hwloc support compiled in)
   catalyst/async/worker_pinned_core (int: core worker is pinned to, -1 if not pinned)
   catalyst/async/stats/timesteps_processed (int: number of timesteps processed)
   catalyst/async/stats/timesteps_skipped   (int: number of timesteps skipped)
   catalyst/async/stats/execute_errors      (int: number of execute errors)

Additional query and statistics functions are available via
``catalyst_async.h``:

.. code-block:: c

   int catalyst_async_has_pending_work(void);
   size_t catalyst_async_queue_depth(void);
   void catalyst_async_get_stats(conduit_node* stats);



Lifecycle
=========

.. code-block:: text

   catalyst_initialize(params)
     ├─ Load implementation (existing behavior)
     ├─ impl->initialize(params)
     └─ catalyst_async_initialize(params)   ← Reads config, starts worker

   catalyst_execute(params)      [called every timestep]
     ├─ If async disabled: impl->execute(params) directly
     └─ If async enabled:
          ├─ Check queue, MPI sync skip decision
          ├─ compact_to() deep copy
          ├─ Enqueue to worker thread
          └─ Return immediately

   catalyst_finalize(params)
     ├─ catalyst_async_finalize()           ← Flush queue, join worker
     └─ impl->finalize(params)

``catalyst_finalize()`` always drains the queue before finalizing the
implementation, ensuring all queued work is processed.


Usage Examples
==============

**Enabling async via environment (recommended for deployment):**

.. code-block:: bash

   export CATALYST_ASYNC_ENABLED=1
   export CATALYST_ASYNC_QUEUE_DEPTH=2
   export CATALYST_ASYNC_VERBOSE=1

   # Optional: explicit worker core assignment
   export CATALYST_ASYNC_WORKER_CORES="1,2,3,4"

   mpirun -np 4 ./my_simulation

**Enabling async via params (recommended for testing):**

.. code-block:: cpp

   conduit::Node params;
   params["catalyst_load/implementation"] = "paraview";
   params["catalyst/async/enabled"] = 1;
   params["catalyst/async/queue_depth"] = 2;
   params["catalyst/async/verbose"] = 1;
   catalyst_initialize(conduit::c_node(&params));

**Adaptor code — no changes needed:**

.. code-block:: cpp

   void my_adaptor_execute(Solver& solver)
   {
     conduit::Node mesh;
     // ... build Conduit Blueprint mesh from solver data ...

     catalyst_execute(conduit::c_node(&mesh));
     // Returns immediately in async mode.
     // Data was deep-copied; solver can safely advance.
   }

**Adaptor that needs synchronization (e.g., for steering):**

.. code-block:: cpp

   void my_adaptor_execute(Solver& solver)
   {
     conduit::Node mesh;
     build_mesh(solver, mesh);
     catalyst_execute(conduit::c_node(&mesh));

     if (solver.needs_steering())
     {
       // Flush via params - wait for all pending work to complete
       conduit::Node flush_params;
       flush_params["catalyst/async/flush"] = 1;
       catalyst_execute(conduit::c_node(&flush_params));

       conduit::Node results;
       catalyst_results(conduit::c_node(&results));
       apply_steering(solver, results);
     }
   }


Design Considerations
=====================

**Why C++17 std::thread instead of pthreads or OpenMP?**
   Catalyst requires C++17 (``cxx_std_17`` is a public compile feature).
   ``std::thread``, ``std::mutex``, ``std::scoped_lock``, and
   ``std::optional`` are portable across Linux, macOS, and Windows without
   external dependencies. C++17 class template argument deduction (CTAD)
   eliminates boilerplate on lock guards, ``std::optional`` replaces sentinel
   values for cleaner error handling, and ``std::string_view`` avoids
   unnecessary copies in configuration parsing. ``inline constexpr``
   variables allow namespace-scope constants without ODR concerns.
   OpenMP is designed for fork-join parallelism (parallel loops), not
   persistent worker threads with producer-consumer queues. Pthreads would
   work but requires ``pthreads-win32`` on Windows and is more verbose.

**Why deep copy instead of reference counting?**
   Conduit nodes created with ``set_external()`` hold raw pointers into
   simulation memory. If the simulation modifies that memory before the
   worker thread processes the node, the result is corrupted data or a
   crash. ``compact_to()`` converts all external references to owned copies,
   making the work item fully independent of simulation memory. The copy
   cost is measured in milliseconds for typical meshes and is acceptable
   for the target use case (GPU simulations where CPU cores are idle during
   compute kernels). Work items are move-only (``WorkItem`` deletes its copy
   constructor) and transferred into the queue via ``std::move``, avoiding
   any redundant copies of the already-compacted Conduit node.

**Why MPI_Allreduce for skip decisions?**
   Catalyst implementations like ParaView use MPI collectives internally.
   If rank 0 enqueues step 10 but rank 1 skips it (because its queue was
   full), the two ranks will enter different MPI collectives and deadlock.
   A single-integer ``MPI_Allreduce(MPI_MIN)`` ensures unanimous
   enqueue/skip decisions at negligible cost. This is only compiled in when
   ``CATALYST_USE_MPI`` is enabled.

**Why impl_mutex for about() and results()?**
   Catalyst implementations may not be thread-safe for concurrent access
   to their internal state. If the main thread calls ``catalyst_about()``
   while the worker thread is inside ``impl->execute()``, both may access
   shared state simultaneously. The mutex serializes these calls. In
   practice, ``about()`` and ``results()`` are rarely called during the
   simulation loop, so there is no meaningful performance impact.

**Queue full policy.**
   The default policy is ``drop_newest``: when the queue is full, the current
   timestep is skipped rather than blocking the simulation. This preserves
   the non-blocking property of async mode.

**Memory budget.**
   With a queue depth of 2, the async layer holds up to 2 additional copies
   of the simulation mesh in memory. For large meshes this can be significant.
   The queue depth should be tuned based on available memory. A depth of 1
   minimizes memory overhead while still enabling overlap.


Build Options
=============

The async layer is always compiled into ``libcatalyst``. It has no runtime cost
when async mode is not enabled (all functions check ``g_initialized`` and return
immediately).

.. list-table::
   :header-rows: 1
   :widths: 40 15 45

   * - CMake Option
     - Default
     - Description
   * - ``CATALYST_ASYNC_USE_HWLOC``
     - ``OFF``
     - Use hwloc for topology detection and cross-platform thread affinity
   * - ``CATALYST_USE_MPI``
     - ``OFF``
     - Enables MPI-synchronized skip decisions (existing option)

The ``Threads::Threads`` dependency is always required (added via
``find_package(Threads REQUIRED)``). If ``CATALYST_ASYNC_USE_HWLOC`` is
enabled, hwloc must be found or CMake will fail.


Statistics
==========

When ``CATALYST_ASYNC_VERBOSE=1`` is set, the async layer prints a summary
at finalization:

.. code-block:: text

   ======= CATALYST ASYNC STATISTICS =======
   Mode: Asynchronous
   Queue depth limit: 2
   Timesteps processed: 48
   Timesteps skipped: 2
   Execute errors: 0
   Slow executes (>10s): 1
   Max queue depth seen: 2
   Total copy time: 0.124000 s
   Total execute time: 12.340000 s
   Max execute time: 11.200000 s
   Avg copy per output: 2.583333 ms
   Avg execute per output: 257.083333 ms
   Max queue wait: 0.003200 s
   =========================================

These statistics are also available programmatically via
``catalyst_async_get_stats()``.


Error Handling
==============

The async worker thread is designed to be resilient:

**Exceptions.** If the implementation throws an exception during
``impl->execute()``, the worker catches it, logs a message to stderr,
increments the error counter, and continues processing subsequent work
items. This prevents a single bad timestep from crashing the entire
simulation.

**Error return status.** If ``impl->execute()`` returns a non-OK status,
the worker logs a warning (in verbose mode) and increments the error
counter. Processing continues normally.

**Slow executions.** If an execute call exceeds ``slow_threshold`` seconds
(default 10), it is logged (in verbose mode) and counted. This helps
identify performance regressions or unexpectedly expensive timesteps.

**Flush timeout.** Flush operations (via ``catalyst/async/flush`` param or
``catalyst_finalize()``) wait for pending work to complete. If the worker
is hung (e.g., implementation deadlock), waiting forever would hang the
simulation. The ``flush_timeout`` setting (default 300 seconds) limits this
wait. If timeout occurs, a warning is printed with the current queue depth
and worker state, and the function returns. This allows the simulation to
exit gracefully rather than hanging indefinitely.
