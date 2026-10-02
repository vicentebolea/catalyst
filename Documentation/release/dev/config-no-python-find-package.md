## Consumers no longer need to find Python

`catalyst-config.cmake` previously called `find_package(Python3 COMPONENTS
Development REQUIRED)` whenever Catalyst was built with `CATALYST_WRAP_PYTHON`.
This forced every downstream project to locate Python development files in order
to configure, even when it made no use of Python itself.

`libcatalyst` no longer links Python on Linux and macOS, and the exported target
reference does not need the package to be found in order to be consumed, so the
call has been removed.
