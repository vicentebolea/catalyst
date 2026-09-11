## `libcatalyst` no longer links against `libpython`

When built with `CATALYST_WRAP_PYTHON`, `libcatalyst` previously linked directly
against `libpython`. The single Python entry point it uses is now resolved at
runtime, so `libcatalyst` links and loads without a matching `libpython` present.
Plain C, C++ and Fortran simulations are unaffected by the Python configuration of
the `libcatalyst` they link against.

For users embedding Catalyst in an existing Python interpreter, this fixes crashes
on macOS caused by two `libpython` instances being loaded into a single process.

Windows behaviour is unchanged. See `docs/runtime_linkage.rst` for the rationale.
