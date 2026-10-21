## Async execution checks the MPI thread level, and flush waits reliably

Async execution has two threads making MPI calls at once: the simulation's
thread and the worker thread running the implementation. MPI allows that only
under `MPI_THREAD_MULTIPLE`. `catalyst_initialize()` now checks the level the
simulation initialized MPI with, and below `MPI_THREAD_MULTIPLE` it prints a
warning and runs synchronously. Initialize MPI with
`MPI_Init_thread(..., MPI_THREAD_MULTIPLE, ...)` to use async execution.

A `catalyst/async/flush` request is no longer passed to the implementation as an
empty timestep, and it now waits even when the queue is full. Before, a flush
issued with a full queue was skipped and returned without waiting, so a
`catalyst_results()` call after it could read results that were not ready.
