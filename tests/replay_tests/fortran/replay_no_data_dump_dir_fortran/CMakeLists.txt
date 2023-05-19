cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(CATALYST_REPLAY_NO_DATA_DUMP_DIR_FORTRAN)
enable_language(Fortran)

find_package(catalyst
  REQUIRED
  COMPONENTS SDK)

# No need to repeat the same adaptor and driver multiple times
# across tests
get_filename_component(PARENT_DIR0 "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
get_filename_component(PARENT_DIR "${PARENT_DIR0}" DIRECTORY)
set(src_dir "${PARENT_DIR}/common_src_dir")

catalyst_implementation(
  TARGET  no_data_dump_dir_adapter
  NAME    replay
  SOURCES "${src_dir}/common_replay_adaptor.cxx")

include(CTest)
if (BUILD_TESTING)
  add_executable(no_data_dump_dir_driver "${PARENT_DIR0}/common_src_dir/common_replay_driver.F90")

  target_link_libraries(no_data_dump_dir_driver
    PRIVATE
      catalyst::catalyst_fortran
      )

  set(num_ranks_if_mpi 2)
  if (CATALYST_USE_MPI)
    find_package(MPI REQUIRED COMPONENTS Fortran)
    target_link_libraries(no_data_dump_dir_driver PRIVATE MPI::MPI_Fortran)
    target_compile_definitions(no_data_dump_dir_driver
      PRIVATE CATALYST_USE_MPI)

    set(mpi_prefix_or_blank
        ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${num_ranks_if_mpi})
  endif()

  # Not specifying a data_dump_dir should be fine when calling the stub implementation.
  # Nothing should be written out
  add_test(
    NAME no_write_out
    COMMAND ${mpi_prefix_or_blank} "$<TARGET_FILE:no_data_dump_dir_driver>"
            "$<TARGET_FILE_DIR:no_data_dump_dir_adapter>"
  )

  set_tests_properties(no_write_out
    PROPERTIES
      FIXTURES_SETUP no_write_out_fixture)

  # Add a test to check behavior when no data_dump_dir is specified to replay
  # command. Should error out.
  add_test(
    NAME no_data_dump_dir_read_in
    COMMAND ${mpi_prefix_or_blank} ${catalyst_replay_command}
  )

  set_tests_properties(no_data_dump_dir_read_in
    PROPERTIES
      PASS_REGULAR_EXPRESSION "ERROR: Expected 1 argument, the data dump directory"
      FIXTURES_REQUIRED no_write_out_fixture)

endif()
