## conduit-external-support

An external Conduit is now supported via the `CATALYST_WITH_EXTERNAL_CONDUIT`
CMake option. Additionally, there is a `CATALYST_RELOCATABLE_INSTALL` option to
support embedding the build-time Conduit path into the install tree to ease
usage of the package at the cost of not being able to be moved to any other
path reliably.

Note that at least Conduit 0.8.6 is required to provide all of the symbols
required.
