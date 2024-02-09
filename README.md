# Catalyst

**Catalyst** is an **API specification** developed for simulations (and other scientific data producers)
to analyze and visualize data in situ.

## ABI commitment

Catalyst is committed to a stable ABI so that existing users can always use a
newer version of the library. To this end, it is very strict about the API
functions available through it. For example, Conduit is embedded, but only its
C API is available (though it is rewrapped for easier use in C++).

## History

Originally, Catalyst was part of [ParaView][paraview]. However, with ParaView
version 5.9, the Catalyst API was revamped quite extensively and release as an
[independent project][catalyst]. This repository includes the definition
together with a lightweight implementation of this revamped Catalyst API.

The documentation describing the details, including build instructions and how to use Catalyst
for in situ analysis and visualization in simulations is available
[on readthedocs][catalyst-docs].

[paraview]: https://gitlab.kitware.com/paraview/paraview
[catalyst]: https://gitlab.kitware.com/paraview/catalyst
[catalyst-docs]: https://catalyst-in-situ.readthedocs.io/en/latest/index.html
