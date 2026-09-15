## `libcatalyst` no longer links MPI

`libcatalyst` previously linked MPI directly and named MPI constants whose
representation differs between implementations, which tied a given `libcatalyst`
to the MPI it was built against. MPI now lives behind a small loadable module,
`libcatalyst_mpi`, built against a real `mpi.h` and installed alongside the
Catalyst implementations.

For simulation developers, nothing changes in the API: the communicator is still
passed as a Fortran handle via `catalyst/mpi_comm`, and adaptors need no
modification.

For packagers, a Catalyst built with `CATALYST_USE_MPI=ON` now installs
`libcatalyst_mpi` next to the implementations and requires it at runtime. If it
cannot be loaded, `catalyst_initialize` fails with
`catalyst_status_error_not_found` rather than continuing with every rank behaving
as rank 0.

See `docs/runtime_linkage.rst` for the rationale and for what this does not
change.
