Architecture
############

This section describes the internal architecture of Catalyst, including the
runtime dispatch mechanism, implementation loading, and the optional
asynchronous execution mode.

.. contents:: Table of Contents
   :local:
   :depth: 2

Overview
********

Catalyst is designed as a **thin dispatch layer** between simulation codes and
in situ processing implementations. Its architecture enforces a strict
separation between the API consumed by simulations and the implementations that
perform actual data analysis and visualization.

.. code-block:: text

   ┌────────────────────────────────────────────────────────┐
   │              Simulation Code                          │
   │         (NekRS, FUN3D, MINT, etc.)                    │
   └──────────────────────┬─────────────────────────────────┘
                          │  conduit_node*
                          ▼
   ┌────────────────────────────────────────────────────────┐
   │                   Adaptor                             │
   │    Builds Conduit Blueprint mesh, calls Catalyst API  │
   └──────────────────────┬─────────────────────────────────┘
                          │  catalyst_execute()
                          ▼
   ┌────────────────────────────────────────────────────────┐
   │              libcatalyst (catalyst_api.c)              │
   │                                                       │
   │  ┌─────────────────────────────────────────────────┐  │
   │  │  Runtime Dispatch                               │  │
   │  │  • dlopen / LoadLibrary                         │  │
   │  │  • Function pointer table (catalyst_impl)       │  │
   │  └─────────────────────────────────────────────────┘  │
   │  ┌─────────────────────────────────────────────────┐  │
   │  │  Async Layer (optional)                         │  │
   │  │  • Worker thread, bounded queue                 │  │
   │  │  • Deep copy via compact_to()                   │  │
   │  │  • MPI-synchronized skip decisions              │  │
   │  └─────────────────────────────────────────────────┘  │
   └──────────────────────┬─────────────────────────────────┘
                          │  impl->execute()
                          ▼
   ┌────────────────────────────────────────────────────────┐
   │           Implementation Library                      │
   │  (libcatalyst-paraview.so, libcatalyst-stub.so, etc.) │
   └────────────────────────────────────────────────────────┘



Runtime Dispatch
****************

The core of Catalyst is ``catalyst_api.c``, which maintains a static pointer to
a ``catalyst_impl`` structure:

.. code-block:: c

   struct catalyst_impl
   {
     int version;
     enum catalyst_status (*initialize)(const conduit_node*);
     enum catalyst_status (*execute)(const conduit_node*);
     enum catalyst_status (*finalize)(const conduit_node*);
     enum catalyst_status (*about)(conduit_node*);
     enum catalyst_status (*results)(conduit_node*);
     conduit_uint64 conduit_is_external;
   };

When ``catalyst_initialize()`` is called, Catalyst locates and loads an
implementation library using the following search order:

1. The name specified in ``params["catalyst_load/implementation"]``
2. The ``CATALYST_IMPLEMENTATION_NAME`` environment variable
3. If neither is set, the built-in stub implementation is used

The implementation library is found by searching:

1. Paths in ``params["catalyst_load/search_paths"]``
2. Paths in ``CATALYST_IMPLEMENTATION_PATHS`` environment variable
3. The ``catalyst/`` directory adjacent to ``libcatalyst``

Once loaded, the library's exported ``catalyst_api_impl`` symbol provides the
function pointer table. Catalyst validates the version and Conduit ABI
compatibility before accepting the implementation.

This design means a simulation compiled against the stub can switch to
ParaView Catalyst (or any other implementation) at runtime by simply setting
environment variables, without recompiling.


Key Source Files
================

``src/catalyst/catalyst_api.c``
   The dispatch layer. Loads implementations via ``dlopen``/``LoadLibrary``,
   maintains the global ``impl`` pointer, and routes all API calls through
   the function pointer table.

``src/catalyst/catalyst_impl.h``
   Defines the ``catalyst_impl`` structure that implementations must export.

``src/catalyst/catalyst_stub.cpp``
   The default (stub) implementation. Optionally dumps Conduit nodes for
   debugging.

``src/catalyst/catalyst_api_default.cpp``
   Wires the stub functions into a ``catalyst_impl`` structure as the
   compile-time default.

``src/catalyst/catalyst_async.cpp``
   The async execution layer. See :doc:`async_execution` for details.
