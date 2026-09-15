# Deliberately not built with `catalyst_implementation()`; the name matters.
# See docs/runtime_linkage.rst.

if (WIN32)
  set(catalyst_mpi_output_base "${CMAKE_INSTALL_BINDIR}")
  set(catalyst_mpi_destination "${CMAKE_INSTALL_BINDIR}/catalyst")
else ()
  set(catalyst_mpi_output_base "${CMAKE_INSTALL_LIBDIR}")
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

# Match the install layout so one search path works in both trees. `$<CONFIG>` is
# spelled out because CMake would otherwise append it below `catalyst/`.
get_property(catalyst_mpi_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (catalyst_mpi_multi_config)
  set(catalyst_mpi_config_dir "/$<CONFIG>")
else ()
  set(catalyst_mpi_config_dir "")
endif ()

set_target_properties(catalyst_mpi
  PROPERTIES
    OUTPUT_NAME "catalyst_mpi"
    C_VISIBILITY_PRESET hidden
    # A MODULE is the LIBRARY artifact on every platform, Windows included.
    LIBRARY_OUTPUT_DIRECTORY
      "${CMAKE_BINARY_DIR}/${catalyst_mpi_output_base}${catalyst_mpi_config_dir}/catalyst"
    RUNTIME_OUTPUT_DIRECTORY
      "${CMAKE_BINARY_DIR}/${catalyst_mpi_output_base}${catalyst_mpi_config_dir}/catalyst")

# Not in the `Catalyst` export set: this module is dlopened, never linked, and
# keeping it out keeps `MPI::MPI_C` out of the exported targets.
install(TARGETS catalyst_mpi
  LIBRARY DESTINATION "${catalyst_mpi_destination}"
  RUNTIME DESTINATION "${catalyst_mpi_destination}")
