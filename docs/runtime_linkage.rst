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
:doc:`compatibility`), threads, and the platform's dynamic loader. Python is
resolved at runtime instead.

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
