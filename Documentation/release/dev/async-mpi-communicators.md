## Async execution no longer shares communicators across threads

In async mode, the skip decisions on the simulation thread and the
implementation's collectives on the worker thread used the same MPI
communicator, also used by the simulation. MPI matches the collectives on a
communicator in call order, whatever the thread, so collectives of the two
threads could match each other across ranks: with ParaView Catalyst on several
ranks, collectives silently returned wrong results.

`catalyst_initialize()` now duplicates the communicator in async mode, once for
the skip decisions and once for the implementation, which receives it as
`catalyst/mpi_comm`. `catalyst_finalize()` frees both. Adaptors need no changes,
but must initialize MPI with `MPI_THREAD_MULTIPLE` to use async mode, as now
documented.

`libcatalyst_mpi` provides two new operations for this, so its operations
version is now 2: `libcatalyst` and `libcatalyst_mpi` must come from the same
Catalyst build, as they are installed.
