Build and Install
#################

Obtaining the source
********************

To obtain the Catalyst source locally, clone the official
code repository using `Git`_.

.. code-block:: bash

    git clone https://gitlab.kitware.com/paraview/catalyst.git


Building
********

Catalyst uses CMake to generate build system scripts and projects, such as
Makefiles or Ninja build files. While IDE generators (Xcode and Visual Studio)
are supported, `Ninja`_ is highly recommended.

To do a fresh build, start with an empty directory as follows:

.. code-block:: bash

    mkdir catalyst-build
    cd catalyst-build
    ccmake -G Ninja [path to catalyst source directory]

    # do the build
    cmake --build .

    # optionally, run tests
    ctest

    # do the install
    cmake --install .


``ccmake`` is a graphical GUI that lets you specify various options for CMake.
On Windows ``cmake-gui`` can be used.
Alternately, those options can be specified on command line to ``cmake`` using
``-Doption:type=value`` (or ``-Doption=value``) parameters as follows:

.. code-block:: bash

    cmake -G Ninja -DCATALYST_BUILD_TESTING:BOOL=ON ... [path to catalyst src dir]


Using ``-G Ninja`` results in CMake generating build files for Ninja. You can
switch to using any other supported generator of your choice. See `CMake Docs`_
for details.

Supported CMake Options
=======================

Important `CMake`_ options that affect how Catalyst is built are:

* ``CATALYST_BUILD_SHARED_LIBS`` (default: ``ON``): choose whether to build static
  or shared libraries for Catalyst. To support switching of Catalyst
  implementation at runtime, you must build with ``CATALYST_BUILD_SHARED_LIBS``
  set to ``ON`` (default).

* ``CATALYST_BUILD_STUB_IMPLEMENTATION`` (default: ``ON``): choose whether to build
  the stub Catalyst implementation. When building Catalyst only to develop
  another Catalyst API implementation, you may turn this option to ``OFF``. If
  ``OFF``, no ``catalyst`` library will be built.

* ``CATALYST_BUILD_TESTING`` (default: ``ON``): enable/disable testing. Running the
  tests using ``ctest`` after a build has succeeded is a good way to verify that
  your build is functional.

* ``CMAKE_BUILD_TYPE`` (default: ``Debug``): this is used to choose whether to add
  debugging symbols to the build. Supported values are ``Debug``, ``Release``,
  ``MinSizeRel``, and ``RelWithDebInfo``.

* ``CMAKE_INSTALL_PREFIX``: path where to install the libraries and headers when
  requested.

* ``CATALYST_WITH_EXTERNAL_CONDUIT`` (default: ``OFF``): Build Catalyst
  against an external Conduit library. Note that this option affects
  implementation compatibility -- an implementation built against a
  Catalyst with external Conduit will refuse to initialize from a Catalyst
  with the internal Conduit and vice versa. See :doc:`compatibility` for
  a full explanation.

* ``CATALYST_RELOCATABLE_INSTALL`` (default: ``ON``): If unset, and to any
  external dependencies will be embedded into the install tree rendering it
  unable to be relocated to other machines without similar setup.

* ``CATALYST_WRAP_PYTHON`` (default: ``OFF``): Build python wrappers for
  Catalyst and Conduit.

* ``CATALYST_WRAP_FORTRAN`` (default: ``OFF``): Build Fortran wrappers for
  Catalyst and Conduit.


.. _`Git`: https://git-scm.com/
.. _`CMake`: https://cmake.org
.. _`CMake Docs`: https://cmake.org/documentation/
.. _`Ninja`: https://ninja-build.org
