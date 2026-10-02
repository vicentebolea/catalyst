cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
project(CATALYST_REPLAY_HIGH_NUM_RANKS_PYTHON)

find_package(catalyst
  REQUIRED
  COMPONENTS SDK)

# No need to repeat the same adaptor and driver multiple times
# across tests
get_filename_component(PARENT_DIR0 "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
get_filename_component(PARENT_DIR "${PARENT_DIR0}" DIRECTORY)
set(src_dir "${PARENT_DIR}/common_src_dir")

catalyst_implementation(
  TARGET  high_num_ranks_adaptor
  NAME    replay
  SOURCES "${src_dir}/common_replay_adaptor.cxx")

include(CTest)
if (BUILD_TESTING)
  find_package(Python3 REQUIRED)
  # These tests launch through MPIEXEC_EXECUTABLE; Catalyst does not provide it.
  if (CATALYST_USE_MPI)
    find_package(MPI COMPONENTS C REQUIRED)
  endif ()
  set(high_num_ranks_driver "${PARENT_DIR0}/common_src_dir/common_replay_driver.py")
  set(num_ranks 10)
  set(mpi_prefix
      ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${num_ranks})

  # Set up the data dump directory for the test
  set(data_dump_directory "${CMAKE_CURRENT_BINARY_DIR}/data_dump/")

  add_test(
    NAME high_num_ranks_write_prepare
    COMMAND "${CMAKE_COMMAND}" -E rm -rf "${data_dump_directory}")

  set_tests_properties(high_num_ranks_write_prepare
    PROPERTIES
      FIXTURES_SETUP prepare)

  # Add a test for writing out the conduit nodes to disk
  add_test(
    NAME high_num_ranks_write_out
    COMMAND  ${mpi_prefix}
             ${Python3_EXECUTABLE} ${high_num_ranks_driver}
            "$<TARGET_FILE_DIR:high_num_ranks_adaptor>" "--use-mpi"
  )
  catalyst_apply_python_env(high_num_ranks_write_out)

  set_tests_properties(high_num_ranks_write_out
    PROPERTIES
      ENVIRONMENT "CATALYST_DATA_DUMP_DIRECTORY=${data_dump_directory}"
      FIXTURES_SETUP write_out
      FIXTURES_REQUIRED prepare)

  add_test(
    NAME high_num_ranks_read_in
    COMMAND ${mpi_prefix} ${catalyst_replay_command} ${data_dump_directory}
  )

  set_tests_properties(high_num_ranks_read_in
    PROPERTIES
      FIXTURES_REQUIRED write_out)

endif()
