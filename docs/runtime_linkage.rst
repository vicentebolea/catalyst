Runtime Linkage
===============

``libcatalyst`` is the library a simulation links against. The analysis work
happens elsewhere, in an implementation such as ``libcatalyst-paraview.so``
that ``catalyst_load`` opens at runtime.

That split only holds while ``libcatalyst`` stays neutral about the libraries
the implementation brings with it. A simulation commonly links a
``libcatalyst`` built locally against a ParaView built somewhere else, by
someone else, with no coordination between the two. Every library
``libcatalyst`` names at link time becomes a constraint on which
implementations it can be paired with.

So ``libcatalyst`` links only what it needs for its own work: Conduit (see
:doc:`compatibility`), threads, and the platform's dynamic loader. Python and
MPI are resolved at runtime instead.

.. contents:: Contents
   :local:
   :depth: 2

Python
------

``libcatalyst`` uses exactly one Python entry point: the capsule bootstrap in
Conduit's ``import_conduit()`` helper, reached only from
``PyCatalystConduit_Node_Wrap``. Everything else Conduit exposes to Python
dispatches through the capsule's function table and was never a link-time
symbol.

That single call site is only reached from a process that has already
initialized an interpreter, so ``PyCapsule_Import`` is looked up with
``dlsym(RTLD_DEFAULT, ...)`` at the point of use. It is part of Python's
stable ABI and its symbol name is unversioned across CPython 3.x, so the
lookup is not tied to a particular Python version.

Linking ``libpython`` instead would load a second CPython runtime into any
process that already has an interpreter. On macOS the two-level namespace
binds Catalyst's calls to its own copy rather than the host interpreter's,
leaving two runtimes in one process, which is fatal.

Windows links the Python library, because extension modules on that platform
require it. This follows CMake's ``Development.Module`` component rather than
a hardcoded platform list.

What this means in practice:

- A ``libcatalyst`` built with ``CATALYST_WRAP_PYTHON=ON`` links and loads
  without a matching ``libpython`` present.
- Plain C, C++ and Fortran simulations are unaffected by the Python
  configuration of the ``libcatalyst`` they link against.
- Consumers of the installed CMake package do not need to find Python
  themselves.
- ``PyCatalystConduit_Node_Wrap`` returns ``nullptr`` when no interpreter is
  present in the process, which is its existing contract.

One limit worth knowing: this covers ``libcatalyst``. An implementation may
still link ``libpython`` itself, as ``libcatalyst-paraview.so`` in the ParaView
bundle does, so a full pipeline driven from an arbitrary interpreter can still
end up with two runtimes in the process. Where two are live, the ``dlsym``
lookup resolves to whichever was loaded first.

MPI
---

``libcatalyst`` names no MPI type or constant. MPI lives behind
``libcatalyst_mpi``, a small module compiled against a real ``mpi.h`` and
installed alongside the Catalyst implementations, which ``libcatalyst`` loads
during ``catalyst_initialize``.

A module rather than per-symbol lookup, because ``MPI_COMM_WORLD``,
``MPI_INT`` and ``MPI_MIN`` are not symbols. They are compile-time values with
no stable representation across implementations: Open MPI defines them as
addresses of global objects, MPICH as integer literals. ``dlsym`` cannot
recover them, so they have to come from a header. Only Fortran communicator
handles and plain ints cross the boundary, and those do have a stable
representation.

Two details that are easy to trip over. The module is named with an
underscore rather than the ``catalyst-<name>`` pattern that ``handle_open()``
builds, so ``CATALYST_IMPLEMENTATION_NAME=mpi`` can never resolve to it. And
it is loaded with ``RTLD_GLOBAL``, because the implementation loaded
afterwards expects its MPI symbols to be reachable.

What this means in practice:

- A ``libcatalyst`` built with ``CATALYST_USE_MPI=ON`` has no MPI in its link
  line, so it is not tied to the MPI it was built against.
- That build installs ``libcatalyst_mpi`` next to the implementations and
  requires it at runtime. If it cannot be loaded, ``catalyst_initialize``
  fails with ``catalyst_status_error_not_found`` rather than continuing with
  every rank behaving as rank 0.
- The simulation still passes its communicator as a Fortran handle via
  ``catalyst/mpi_comm``, so no adaptor change is needed.
- This package no longer provides ``MPI::MPI_C``. An adaptor that uses MPI
  directly finds it itself, as it would any other dependency of its own.

This does not let a simulation using one MPI interoperate with an
implementation using another. There is still only one MPI in a process. What
changes is that ``libcatalyst`` no longer decides which one it is.
