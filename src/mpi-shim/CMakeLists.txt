# The MPI shim is the only part of Catalyst compiled against a real `mpi.h`.
# `libcatalyst` itself names no MPI type or constant and loads this module at
# runtime, so a single `libcatalyst` binary works against whichever MPI is
# present.
#
# It is deliberately NOT built with `catalyst_implementation()`. It shares the
# implementation directory so that the existing self-location logic finds it,
# but uses an underscore (`libcatalyst_mpi`) rather than the `catalyst-<name>`
# pattern `handle_open()` builds, so `CATALYST_IMPLEMENTATION_NAME=mpi` can
# never resolve to it.

if (WIN32)
  set(catalyst_mpi_destination "${CMAKE_INSTALL_BINDIR}/catalyst")
else ()
  set(catalyst_mpi_destination "${CMAKE_INSTALL_LIBDIR}/catalyst")
endif ()

add_library(catalyst_mpi MODULE
  catalyst_mpi_shim.c)

target_link_libraries(catalyst_mpi
  PRIVATE
    MPI::MPI_C)

target_include_directories(catalyst_mpi
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../catalyst"
    "${CMAKE_CURRENT_BINARY_DIR}/../catalyst")

set_target_properties(catalyst_mpi
  PROPERTIES
    OUTPUT_NAME "catalyst_mpi"
    C_VISIBILITY_PRESET hidden
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${catalyst_mpi_destination}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${catalyst_mpi_destination}")

# Deliberately not in the `Catalyst` export set: this module is dlopened, never
# linked, so exporting it would commit us to a public target name for no gain.
# Keeping it out also keeps `MPI::MPI_C` out of the exported targets entirely.
install(TARGETS catalyst_mpi
  LIBRARY DESTINATION "${catalyst_mpi_destination}"
  RUNTIME DESTINATION "${catalyst_mpi_destination}")
