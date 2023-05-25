cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
project(CATALYST_REPLAY_MISSING_INITIALIZE_DATA_PYTHON)

find_package(catalyst
  REQUIRED
  COMPONENTS SDK)

# No need to repeat the same adaptor and driver multiple times
# across tests
get_filename_component(PARENT_DIR0 "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
get_filename_component(PARENT_DIR "${PARENT_DIR0}" DIRECTORY)
set(src_dir "${PARENT_DIR}/common_src_dir")

catalyst_implementation(
  TARGET  missing_initialize_adaptor
  NAME    replay
  SOURCES "${src_dir}/common_replay_adaptor.cxx")

include(CTest)
if (BUILD_TESTING)
  find_package(Python3 REQUIRED)
  set(missing_initialize_driver "${PARENT_DIR0}/common_src_dir/common_replay_driver.py")

  set(num_ranks_if_mpi 2)
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
    NAME missing_initialize_prepare
    COMMAND "${CMAKE_COMMAND}" -E rm -rf "${data_dump_directory}")

  set_tests_properties(missing_initialize_prepare
    PROPERTIES
      FIXTURES_SETUP prepare_fixture)

  add_test(
    NAME missing_initialize_write_out
    COMMAND  ${CMAKE_COMMAND} -E env --modify PYTHONPATH=path_list_prepend:${CATALYST_PYTHONPATH}
             ${mpi_prefix_or_blank}
             ${Python3_EXECUTABLE} ${missing_initialize_driver}
            "$<TARGET_FILE_DIR:missing_initialize_adaptor>" ${mpi_flag_or_blank}
  )

  file(MAKE_DIRECTORY ${data_dump_directory})

  set(write_out_environ_str
     "CATALYST_DATA_DUMP_DIRECTORY=${data_dump_directory}"
  )

  set_tests_properties(missing_initialize_write_out
    PROPERTIES
      ENVIRONMENT "${write_out_environ_str}"
      FIXTURES_SETUP missing_initialize_write_out_fixture
      FIXTURES_REQUIRED prepare_fixture)


  # Remove one of the ranks' initialize data
  if (CATALYST_USE_MPI)
    add_test(NAME remove_initialize
             COMMAND "${CMAKE_COMMAND}" -E rm
                     "${data_dump_directory}/initialize_params.conduit_bin.${num_ranks_if_mpi}.0")
  else()
    add_test(NAME remove_initialize
             COMMAND "${CMAKE_COMMAND}" -E rm
                     "${data_dump_directory}/initialize_params.conduit_bin.1.0")
  endif()

  set_tests_properties(remove_initialize
    PROPERTIES
      FIXTURES_REQUIRED missing_initialize_write_out_fixture
      FIXTURES_SETUP remove_initialize_fixture)


  # Add a test to check behavior when we're missing node data
  # for one rank's initialize stage.
  add_test(
    NAME missing_initialize_read_in
    COMMAND ${mpi_prefix_or_blank} ${catalyst_replay_command} ${data_dump_directory}
  )

  # The expected error message will be different based on how many files we write out,
  # which depends on the number of ranks.
  set(pass_str "ERROR: Missing node data for at least one stage")
  if (CATALYST_USE_MPI AND ${num_ranks_if_mpi} GREATER 1)
    set(pass_str "ERROR: Mismatch in the number of calls to initialize or finalize")
  endif()

  set_tests_properties(missing_initialize_read_in
    PROPERTIES
      PASS_REGULAR_EXPRESSION ${pass_str}
      FIXTURES_REQUIRED remove_initialize_fixture)

endif()
