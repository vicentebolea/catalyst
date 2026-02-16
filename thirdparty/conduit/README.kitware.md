# conduit fork for Catalyst

This branch contains changes required to embed conduit into Catalyst. This
includes changes made primarily to the build system to allow it to be embedded
into another source tree as well as a header to facilitate mangling of the
symbols to avoid conflicts with other copies of the library within a single
process.

  * Ignore whitespace errors for Catalyst's commit checks.
  * Fix file permissions.
  * Simplify CMakeBasics.cmake to remove unnecessary bits.
  * Write a CMake build system for use in Catalyst.
  * Mangle all symbols to avoid conflicts.
  * Add harness for `CATALYST_WRAP_PYTHON` that includes python wrappings.
  * Write CMake build system for python targets for use in Catalyst.
  * Remove `blueprint.{mcarray.mesh,table}.examples` and `relay` python modules from __init__ files.
  * Rename python package to `catalyst_conduit`
  * Add harness for `CATALYST_WRAP_FORTRAN` that includes Fortran bindings.
  * Mangle Fortran C-binding function names to use `c_catalyst_conduit_*` prefix.
  * Hide internal C++ symbols from Catalyst ABI using visibility attributes.
  * Hide yyjson symbols from Catalyst ABI (new dependency in v0.9.5).
  * Add explicit `-fvisibility=hidden` compile flags for older compilers.
  * Add validation scripts for Catalyst integration testing.
