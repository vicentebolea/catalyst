# These tests require that we use num_ranks != 1 at least once.
# Hence, we can not run it unless MPI is enabled
if (CATALYST_USE_MPI)
  add_catalyst_test(replay_high_num_ranks_python)
  add_catalyst_test(replay_ranks_mismatch_python)
endif()

add_catalyst_test(replay_high_num_execute_invc_python)
add_catalyst_test(replay_no_data_dump_dir_python)
add_catalyst_test(replay_missing_initialize_data_python)
add_catalyst_test(replay_missing_execute_invc_python)
