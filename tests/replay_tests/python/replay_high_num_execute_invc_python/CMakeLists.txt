cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
project(CATALYST_REPLAY_HIGH_NUM_EXECUTE_INVC_PYTHON)

find_package(catalyst
  REQUIRED
  COMPONENTS SDK)

# No need to repeat the same adaptor and driver multiple times
# across tests. Here we'll just use the common adaptor.
get_filename_component(PARENT_DIR0 "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
get_filename_component(PARENT_DIR "${PARENT_DIR0}" DIRECTORY)
set(common_src_dir "${PARENT_DIR}/common_src_dir")

catalyst_implementation(
  TARGET  high_num_execute_invc_adaptor
  NAME    replay
  SOURCES "${common_src_dir}/common_replay_adaptor.cxx")

include(CTest)
if (BUILD_TESTING)
  find_package(Python3 REQUIRED)
  set(high_num_execute_invc_driver "${CMAKE_CURRENT_SOURCE_DIR}/high_num_execute_invc_driver.py")

  set(num_ranks_if_mpi 2)
  set(mpi_prefix_or_blank)
  set(mpi_flag_or_blank)
  if (CATALYST_USE_MPI)
    set(mpi_prefix_or_blank
        ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} ${num_ranks_if_mpi})
    set(mpi_flag_or_blank "--use-mpi")
  endif()

  # Set up the data dump directory for the test
  set(data_dump_directory "${CMAKE_CURRENT_BINARY_DIR}/data_dump")

  add_test(
    NAME high_num_execute_invc_prepare
    COMMAND "${CMAKE_COMMAND}" -E rm -rf "${data_dump_directory}")

  set_tests_properties(high_num_execute_invc_prepare
    PROPERTIES
      FIXTURES_SETUP prepare)

  # Add a test for writing out the conduit nodes to disk
  add_test(
    NAME high_num_execute_invc_write_out
    COMMAND  ${mpi_prefix_or_blank}
             ${Python3_EXECUTABLE} ${high_num_execute_invc_driver}
            "$<TARGET_FILE_DIR:high_num_execute_invc_adaptor>" ${mpi_flag_or_blank}
  )
  catalyst_apply_python_env(high_num_execute_invc_write_out)

  set_tests_properties(high_num_execute_invc_write_out
    PROPERTIES
      ENVIRONMENT "CATALYST_DATA_DUMP_DIRECTORY=${data_dump_directory}"
      FIXTURES_SETUP write_out
      FIXTURES_REQUIRED prepare)

  add_test(
    NAME high_num_execute_invc_read_in
    COMMAND ${mpi_prefix_or_blank} ${catalyst_replay_command} ${data_dump_directory}
  )

  set_tests_properties(high_num_execute_invc_read_in
    PROPERTIES
      FIXTURES_REQUIRED write_out)

endif()
