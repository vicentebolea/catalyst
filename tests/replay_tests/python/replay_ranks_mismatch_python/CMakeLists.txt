cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
project(CATALYST_REPLAY_RANKS_MISMATCH_PYTHON)

find_package(catalyst
  REQUIRED
  COMPONENTS SDK)

# No need to repeat the same adaptor and driver multiple times
# across tests
get_filename_component(PARENT_DIR0 "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
get_filename_component(PARENT_DIR "${PARENT_DIR0}" DIRECTORY)
set(src_dir "${PARENT_DIR}/common_src_dir")

catalyst_implementation(
  TARGET  replay_ranks_mismatch_adaptor
  NAME    replay
  SOURCES "${src_dir}/common_replay_adaptor.cxx")

include(CTest)
if (BUILD_TESTING)
  find_package(Python3 REQUIRED)
  # These tests launch through MPIEXEC_EXECUTABLE; Catalyst does not provide it.
  if (CATALYST_USE_MPI)
    find_package(MPI COMPONENTS C REQUIRED)
  endif ()
  set(replay_ranks_mismatch_driver "${PARENT_DIR0}/common_src_dir/common_replay_driver.py")

  # Make sure that we error out if the number of ranks is different
  set(num_ranks_write_out 1)
  set(num_ranks_read_in 2)

  # Set up the data dump directory for the test
  set(data_dump_directory "${CMAKE_CURRENT_BINARY_DIR}/data_dump/")

  add_test(
    NAME replay_ranks_mismatch_prepare
    COMMAND "${CMAKE_COMMAND}" -E rm -rf "${data_dump_directory}")

  set_tests_properties(replay_ranks_mismatch_prepare
    PROPERTIES
      FIXTURES_SETUP prepare_fixture)

  # Add a test for writing out the conduit nodes to disk
  add_test(
    NAME replay_ranks_mismatch_write_out
    COMMAND  ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${num_ranks_write_out}
             ${Python3_EXECUTABLE} ${replay_ranks_mismatch_driver}
            "$<TARGET_FILE_DIR:replay_ranks_mismatch_adaptor>" "--use-mpi"
  )
  catalyst_apply_python_env(replay_ranks_mismatch_write_out)

  set_tests_properties(replay_ranks_mismatch_write_out
    PROPERTIES
      ENVIRONMENT "CATALYST_DATA_DUMP_DIRECTORY=${data_dump_directory}"
      FIXTURES_SETUP replay_ranks_mismatch_write_out_fixture
      FIXTURES_REQUIRED prepare_fixture)

  add_test(
    NAME replay_ranks_mismatch_read_in
    COMMAND ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${num_ranks_read_in}
            ${catalyst_replay_command} ${data_dump_directory}
  )

  set_tests_properties(replay_ranks_mismatch_read_in
    PROPERTIES
      PASS_REGULAR_EXPRESSION "ERROR: Mismatch in the number of ranks"
      FIXTURES_REQUIRED replay_ranks_mismatch_write_out_fixture)

endif()
