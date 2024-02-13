What is Catalyst?
*****************

Catalyst is an API specification developed for simulations (and other
scientific data producers) to analyze and visualize data in situ.

It also includes the following:

* A light-weight implementation of the Catalyst API. This implementation
  is called **stub**.
* An SDK for developers to develop implementations of the Catalyst API to perform
  custom data processing and visualization tasks.

The Catalyst API uses 'C' and is binary compatible with different
implementations of the API making it easier to change the implementation
at runtime.

Relationship with ParaView
==========================

Starting with 5.9, ParaView releases come with an implementation of the Catalyst
API. This implementation can be used in lieu of the **stub** to analyze and
visualize simulation results using ParaView's data-processing and
visualization capabilities.

Relationship with Conduit
=========================

The Catalyst API uses `Conduit`_ for describing data and other parameters
which can be communicated between a simulation and Catalyst.

Conduit provides a standard way to describe computational simulation meshes.
This is called the `Mesh Blueprint`_. ParaView's implementation of the Catalyst
API supports a subset of the Mesh Blueprint. Simulations that can use the Mesh
Blueprint to describe their data can directly use ParaView's Catalyst
implementation for in situ analysis and visualization.

ParaView Catalyst
=================

**ParaView Catalyst** is the name now used to refer to ParaView's implementation of the
Catalyst API. Prior to this API separation (i.e. ParaView 5.8 and earlier),
ParaView Catalyst or simply Catalyst was used to denote the in situ API together
with the data analysis and visualization capabilities it provided. In other words,
the in-situ capabilities of ParaView were collectively called Catalyst.

With ParaView 5.9, while legacy uses will still be supported for a few more
releases, we use the names to refer to specific components:

* **Catalyst**: the API and SDK described here.
* **ParaView**: the parallel data analysis and visualization application and framework.
* **ParaView Catalyst**: the implementation of the Catalyst API that uses ParaView for
  in situ data analysis and visualization.

ParaView Catalyst supports several ways for simulations to describe
computational meshes and fields. One way is to use Conduit's `Mesh Blueprint`_.
Another way is to use `Fides`_.

Furthermore, developers can develop their own
implementations of the Catalyst API and still use ParaView's capabilities for
in situ data processing and visualization. ParaView provides `API`_ that
such developers can use to initialize and invoke ParaView in situ.


.. _`Conduit`: https://llnl-conduit.readthedocs.io/en/latest/index.html

.. _`Mesh Blueprint`: https://llnl-conduit.readthedocs.io/en/latest/blueprint_mesh.html#mesh-blueprint

.. _`Fides`: https://gitlab.kitware.com/vtk/fides

.. _`API`: https://kitware.github.io/paraview-docs/nightly/cxx/group__Insitu.html

Catalyst API Implementations
============================

Known implementations of the Catalyst API include:

* `ParaView Catalyst`_
* `Adios Catalyst`_
* Ascent Catalyst

.. _`ParaView Catalyst` : https://docs.paraview.org/en/latest/Catalyst/index.html
.. _`Adios Catalyst` : https://gitlab.kitware.com/paraview/adioscatalyst

License
========

Catalyst is distributed under the OSI-approved BSD 3-clause License. See
`here`_ for details.

.. _`here`: https://gitlab.kitware.com/paraview/catalyst/-/blob/master/License.txt

Citing
======

When citing Catalyst in your scientific research, please mention the following work to support increased visibility and dissemination of our software:


::

   Ayachit, U., Bauer Andrew C., Boeckel Ben, Geveci Berk, Moreland Kenneth, O`Leary Patrick, Osika Tom  (2021). Catalyst Revised: Rethinking the ParaView in Situ Analysis and Visualization API. In: Jagode, H., Anzt, H., Ltaief, H., Luszczek, P. (eds) High Performance Computing. ISC High Performance 2021. Lecture Notes in Computer Science(), vol 12761. Springer, Cham. https://doi.org/10.1007/978-3-030-90539-2_33

For your convenience here is a bibtex entry:

.. code-block:: bibtex

   @InProceedings{catalyst_revised2021,
     author="Ayachit, Utkarsh and Bauer, Andrew C.  and Boeckel, Ben and Geveci, Berk and Moreland, Kenneth and O'Leary, Patrick and Osika, Tom",
     editor="Jagode, Heike and Anzt, Hartwig and Ltaief, Hatem and Luszczek, Piotr",
     title="Catalyst Revised: Rethinking the ParaView in Situ Analysis and Visualization API",
     booktitle="High Performance Computing",
     year="2021",
     publisher="Springer International Publishing",
     address="Cham",
     pages="484--494",
     isbn="978-3-030-90539-2",
     doi="10.1007/978-3-030-90539-2_33"
   }
