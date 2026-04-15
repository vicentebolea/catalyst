GPU-Resident Workflows
**********************

Catalyst 2.0 supports zero-copy passthrough of GPU-resident simulation data.
Device pointers are passed directly into the Conduit node via ``set_external()``
and the ParaView backend routes them through a Viskores pipeline without
any host copy. This page covers how device detection works and how to write an
adaptor for a GPU simulation.

For overlapping Catalyst processing with GPU compute, see :doc:`async_execution`.

A complete working example is available in the `catalyst-examples`_ repository
under ``ParaView/CxxCudaFullExample``.

.. _catalyst-examples: https://gitlab.kitware.com/paraview/catalyst-examples

How Device Detection Works
==========================

The ParaView backend probes each pointer individually using the CUDA Driver API::

    vtkConduitArrayUtilities::MCArrayToVTKArray()
      └─ IsDevicePointer(ptr, id, working)
           └─ vtkmDataArrayUtilities::IsDevicePointer(ptr, id)
                └─ cuPointerGetAttribute(CUDA_POINTER_ATTRIBUTE_MEMORY_TYPE)

If the pointer is device-resident, the backend creates a ``vtkmDataSet`` backed
by a Viskores ArrayHandle. If it is host-resident, a standard ``vtkDataSet`` is
created instead. This happens per-array, so mixed host/device channels are
supported.

HIP (AMD GPU) pointers are detected via the equivalent ROCm runtime query.
Use ``--viskores-device=HIP`` at initialization for AMD targets.

Initializing the Viskores Backend
==================================

To enable GPU-resident processing, the Viskores backend must be initialized
at ``catalyst_initialize`` time by specifying the target device:

.. code-block:: cpp

    conduit_cpp::Node node;
    // ... script paths and implementation ...
    node["catalyst/viskores/args"].append() = "--viskores-device=CUDA";
    catalyst_initialize(conduit_cpp::c_node(&node));

Available device strings correspond to Viskores-supported backends:

* ``CUDA`` — NVIDIA GPUs
* ``HIP`` — AMD GPUs (ROCm)
* ``Kokkos`` — portable backend (requires Kokkos-enabled build)
* ``OpenMP`` — CPU parallelism via OpenMP
* ``TBB`` — Intel Threading Building Blocks (CPU)

Without this initialization step, device pointers passed via ``set_external()``
will be detected correctly but processed on the CPU via a device-to-host copy.

Writing the Adaptor
===================

The adaptor code is identical to a host-memory adaptor. The only difference is
that the arrays returned by the simulation are ``cudaMalloc``-allocated device
pointers:

.. code-block:: cpp

    void Execute(int cycle, double time, Grid& grid, Attributes& attribs)
    {
      conduit_cpp::Node exec_params;
      auto channel = exec_params["catalyst/channels/grid"];
      channel["type"].set("mesh");

      auto mesh = channel["data"];
      mesh["coordsets/coords/type"].set("explicit");

      // Strided SOA: x/y/z interleaved in a single float3 device buffer.
      // set_external passes the device pointer directly — no copy occurs here.
      mesh["coordsets/coords/values/x"].set_external(
        grid.GetPointsArray(), grid.GetNumberOfPoints(),
        /*offset=*/0, /*stride=*/3 * sizeof(float));
      mesh["coordsets/coords/values/y"].set_external(
        grid.GetPointsArray(), grid.GetNumberOfPoints(),
        /*offset=*/sizeof(float), /*stride=*/3 * sizeof(float));
      mesh["coordsets/coords/values/z"].set_external(
        grid.GetPointsArray(), grid.GetNumberOfPoints(),
        /*offset=*/2 * sizeof(float), /*stride=*/3 * sizeof(float));

      mesh["topologies/mesh/type"].set("unstructured");
      mesh["topologies/mesh/coordset"].set("coords");
      mesh["topologies/mesh/elements/shape"].set("hex");
      mesh["topologies/mesh/elements/connectivity"].set_external(
        grid.GetCellPoints(0), grid.GetNumberOfCells() * 8);

      // Velocity: non-interlaced SOA, three separate device component buffers.
      auto& f = mesh["fields"];
      f["velocity/association"].set("vertex");
      f["velocity/topology"].set("mesh");
      f["velocity/volume_dependent"].set("false");
      f["velocity/values/x"].set_external(
        attribs.GetVelocityArray(), grid.GetNumberOfPoints(), /*offset=*/0);
      f["velocity/values/y"].set_external(
        attribs.GetVelocityArray(), grid.GetNumberOfPoints(),
        /*offset=*/grid.GetNumberOfPoints() * sizeof(float));
      f["velocity/values/z"].set_external(
        attribs.GetVelocityArray(), grid.GetNumberOfPoints(),
        /*offset=*/grid.GetNumberOfPoints() * sizeof(float) * 2);

      // Pressure: cell-centred scalar.
      f["pressure/association"].set("element");
      f["pressure/topology"].set("mesh");
      f["pressure/volume_dependent"].set("false");
      f["pressure/values"].set_external(
        attribs.GetPressureArray(), grid.GetNumberOfCells());

      catalyst_execute(conduit_cpp::c_node(&exec_params));
    }

Always call ``cudaDeviceSynchronize()`` before ``catalyst_execute()`` to ensure
all device writes are complete before the pointers are read.

Pipeline Scripts
================

Use ``VTKm``-prefixed filter variants in pipeline scripts to keep data on the
device. Note that while the Python API uses the ``VTKm`` prefix for historical
reasons, these filters are backed by Viskores:

.. code-block:: python

    LoadDistributedPlugin('VTKmFilters', remote=False, ns=globals())
    producer  = TrivialProducer(registrationName="grid")
    p2cell    = VTKmPointDatatoCellData(Input=producer)
    threshold = VTKmThreshold(Input=p2cell)
    threshold.Scalars = ['CELLS', 'velocity']
    threshold.LowerThreshold = 0
    threshold.UpperThreshold = 15

Using standard ParaView filters (e.g. plain ``Threshold``) will trigger an
implicit device-to-host copy before processing.

Known Limitation: Strided Connectivity
=======================================

Viskores requires a pointer to the end of the connectivity array for stride
computation. If your solver does not provide this directly, the backend falls
back to a device-to-host deep copy of the connectivity array only. Coordinate
and field arrays are not affected. This is tracked for a future fix.
