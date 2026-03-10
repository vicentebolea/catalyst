cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
project(CATALYST_REPLAY_MISSING_EXECUTE_INVC_PYTHON)

find_package(catalyst
  REQUIRED
  COMPONENTS SDK)

# No need to repeat the same adaptor and driver multiple times
# across tests
get_filename_component(PARENT_DIR0 "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
get_filename_component(PARENT_DIR "${PARENT_DIR0}" DIRECTORY)
set(src_dir "${PARENT_DIR}/common_src_dir")

catalyst_implementation(
  TARGET  missing_execute_invc_adaptor
  NAME    replay
  SOURCES "${src_dir}/common_replay_adaptor.cxx")

include(CTest)
if (BUILD_TESTING)
  find_package(Python3 REQUIRED)
  set(missing_execute_invc_driver "${PARENT_DIR0}/common_src_dir/common_replay_driver.py")

  set(num_ranks_if_mpi 1)
  set(mpi_prefix_or_blank)
  set(mpi_flag_or_blank)
  if (CATALYST_USE_MPI)
    set(mpi_prefix_or_blank
        ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${num_ranks_if_mpi})
    set(mpi_flag_or_blank "--use-mpi")
  endif()

  # Set up the data dump directory for the test
  set(data_dump_directory "${CMAKE_CURRENT_BINARY_DIR}/data_dump/")

  add_test(
    NAME missing_execute_invc_prepare
    COMMAND "${CMAKE_COMMAND}" -E rm -rf "${data_dump_directory}")

  set_tests_properties(missing_execute_invc_prepare
    PROPERTIES
      FIXTURES_SETUP prepare_fixture)

  add_test(
    NAME missing_execute_invc_write_out
    COMMAND  ${mpi_prefix_or_blank}
             ${Python3_EXECUTABLE} ${missing_execute_invc_driver}
            "$<TARGET_FILE_DIR:missing_execute_invc_adaptor>"
             ${mpi_flag_or_blank}
  )
  catalyst_apply_python_env(missing_execute_invc_write_out)

  set_tests_properties(missing_execute_invc_write_out
    PROPERTIES
      ENVIRONMENT "CATALYST_DATA_DUMP_DIRECTORY=${data_dump_directory}"
      FIXTURES_SETUP missing_execute_invc_write_out_fixture
      FIXTURES_REQUIRED prepare_fixture)

  # Remove one invocation of execute
  if (CATALYST_USE_MPI)
    add_test(NAME remove_execute_invc0
             COMMAND "${CMAKE_COMMAND}" -E rm
                     "${data_dump_directory}/execute_invc0_params.conduit_bin.${num_ranks_if_mpi}.0")
  else()
    add_test(NAME remove_execute_invc0
             COMMAND "${CMAKE_COMMAND}" -E rm
                     "${data_dump_directory}/execute_invc0_params.conduit_bin.1.0")
  endif()

  set_tests_properties(remove_execute_invc0
    PROPERTIES
      FIXTURES_REQUIRED missing_execute_invc_write_out_fixture
      FIXTURES_SETUP remove_execute_invc0_fixture)


  # Add a test to check behavior when we're missing node data
  # for one execute invocation
  add_test(
    NAME missing_execute_invc_read_in
    COMMAND ${mpi_prefix_or_blank} ${catalyst_replay_command} ${data_dump_directory}
  )

  set_tests_properties(missing_execute_invc_read_in
    PROPERTIES
      PASS_REGULAR_EXPRESSION "ERROR: Unexpected number of calls to execute"
      FIXTURES_REQUIRED remove_execute_invc0_fixture)

endif()
