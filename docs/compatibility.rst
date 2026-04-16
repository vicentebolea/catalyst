Conduit Compatibility
=====================

Catalyst embeds its own private copy of Conduit by default, or can link
against an external Conduit installation. This choice is made at build time
via ``CATALYST_WITH_EXTERNAL_CONDUIT`` and has important compatibility
implications for deployments.

.. contents:: Contents
   :local:
   :depth: 2

External vs Internal Conduit
-----------------------------

**The rule is simple: every component in the stack must be built with the
same** ``CATALYST_WITH_EXTERNAL_CONDUIT`` **setting.**

The two components that must agree are:

- ``libcatalyst.so`` -- the library the simulation links against
- The Catalyst implementation (e.g., ``libcatalyst-paraview.so``) -- loaded
  at runtime by the backend

If these are built with different settings, Catalyst detects the mismatch
at startup and refuses to initialize::

    catalyst error: Conduit configuration mismatch between libcatalyst and
      '/path/to/libcatalyst-paraview.so'.
      libcatalyst built with CATALYST_WITH_EXTERNAL_CONDUIT=1
      implementation built with CATALYST_WITH_EXTERNAL_CONDUIT=0
      Both must be built with the same Conduit configuration.

Why mixing is not supported
~~~~~~~~~~~~~~~~~~~~~~~~~~~

When ``CATALYST_WITH_EXTERNAL_CONDUIT=OFF``, all Conduit symbols are renamed
with a private namespace prefix (``catalyst_conduit_node_*``). A
``conduit_node*`` created by the mangled runtime cannot be correctly
interpreted by the unmangled external runtime, and vice versa. The two
runtimes resolve the same function names to different code, operate on
different heap allocators, and have no shared state. Node operations such as
``has_path()`` will silently return incorrect results, causing
``catalyst_initialize`` to fail without a useful diagnostic.

Recommended configuration
~~~~~~~~~~~~~~~~~~~~~~~~~

Use the default (``CATALYST_WITH_EXTERNAL_CONDUIT=OFF``) and build all
components from the same Catalyst source or release. This is the most robust
option for HPC deployments because:

- No external Conduit installation is needed
- No version coordination is required between components
- Conduit is fully isolated inside each library

Only use ``CATALYST_WITH_EXTERNAL_CONDUIT=ON`` if you have a specific reason
(for example, sharing Conduit with another library in the same process), and
ensure every Catalyst component is built against the same external Conduit
installation.

Internal Conduit Version Mismatch
----------------------------------

When both ``libcatalyst.so`` and the implementation use
``CATALYST_WITH_EXTERNAL_CONDUIT=OFF``, each component embeds its own private
copy of Conduit. If those copies are different versions, there is no
detection mechanism -- both report ``conduit_is_external=0`` and the
compatibility check passes.

If the internal ``conduit_node`` struct layout differs between the two
versions, node operations will silently return incorrect results, causing
``catalyst_initialize`` to fail without a diagnostic.

In practice, the Conduit struct layout has been stable across recent versions.
However, this is not guaranteed across all future releases. The safest
approach is to ensure that ``libcatalyst.so`` and the implementation are
built from the same Catalyst source, which guarantees they embed identical
Conduit versions.
