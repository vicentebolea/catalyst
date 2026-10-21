add_subdirectory(impls)

add_executable(test_double_impl test_double_impl.c)
target_link_libraries(test_double_impl catalyst::catalyst)
add_test(NAME catalyst-impl-double COMMAND test_double_impl)

add_executable(test_about_impl_path test_about_impl_path.c)
target_link_libraries(test_about_impl_path catalyst::catalyst)
add_test(NAME catalyst-about-impl-path COMMAND test_about_impl_path)
catalyst_test_use_stub_backend(catalyst-about-impl-path)

add_executable(test_about_impl_path_double test_about_impl_path_double.c)
target_link_libraries(test_about_impl_path_double catalyst::catalyst)
add_test(NAME catalyst-about-impl-path-double COMMAND test_about_impl_path_double "$<TARGET_FILE:catalyst-double>")
catalyst_test_use_stub_backend(catalyst-about-impl-path-double)

add_executable(test_about_impl_path_params test_about_impl_path_params.c)
target_link_libraries(test_about_impl_path_params catalyst::catalyst)
add_test(NAME catalyst-about-impl-path-params COMMAND test_about_impl_path_params "$<TARGET_FILE_DIR:catalyst-double>")
catalyst_test_use_stub_backend(catalyst-about-impl-path-params)

add_executable(test_about_impl_path_env test_about_impl_path_env.c)
target_link_libraries(test_about_impl_path_env catalyst::catalyst)
add_test(NAME catalyst-about-impl-path-env COMMAND test_about_impl_path_env "$<TARGET_FILE_DIR:catalyst-double>")
catalyst_test_use_stub_backend(catalyst-about-impl-path-env)

add_executable(test_catalyst_results catalyst_results/test_catalyst_results.c)
target_link_libraries(test_catalyst_results catalyst::catalyst)
add_test(NAME catalyst-results COMMAND test_catalyst_results)

add_executable(test_catalyst_conduit catalyst_conduit/test_catalyst_conduit.cpp)
target_link_libraries(test_catalyst_conduit catalyst::catalyst)
add_test(NAME catalyst-conduit COMMAND test_catalyst_conduit)

add_executable(test_external_conduit_impl test_external_conduit_impl.c)
target_compile_definitions(test_external_conduit_impl PRIVATE "CATALYST_WITH_EXTERNAL_CONDUIT=$<BOOL:${CATALYST_WITH_EXTERNAL_CONDUIT}>")
target_link_libraries(test_external_conduit_impl catalyst::catalyst)
add_test(NAME catalyst-impl-external-conduit COMMAND test_external_conduit_impl)

add_executable(test_internal_conduit_impl test_internal_conduit_impl.c)
target_compile_definitions(test_internal_conduit_impl PRIVATE "CATALYST_WITH_EXTERNAL_CONDUIT=$<BOOL:${CATALYST_WITH_EXTERNAL_CONDUIT}>")
target_link_libraries(test_internal_conduit_impl catalyst::catalyst)
add_test(NAME catalyst-impl-internal-conduit COMMAND test_internal_conduit_impl)

add_executable(test_catalyst_async test_catalyst_async.cpp)
target_compile_features(test_catalyst_async PRIVATE cxx_std_17)
target_link_libraries(test_catalyst_async catalyst::catalyst)
add_test(NAME catalyst-async COMMAND test_catalyst_async)
catalyst_test_use_stub_backend(catalyst-async)

if (CATALYST_USE_MPI)
  add_executable(test_catalyst_async_mpi test_catalyst_async_mpi.cpp)
  target_compile_features(test_catalyst_async_mpi PRIVATE cxx_std_17)
  target_link_libraries(test_catalyst_async_mpi catalyst::catalyst MPI::MPI_C)
  add_test(
    NAME    catalyst-async-mpi
    COMMAND ${MPIEXEC_EXECUTABLE} ${MPIEXEC_NUMPROC_FLAG} 4 ${MPIEXEC_PREFLAGS}
            $<TARGET_FILE:test_catalyst_async_mpi> ${MPIEXEC_POSTFLAGS}
            "$<TARGET_FILE_DIR:catalyst-async-mpi>")
  set_tests_properties(catalyst-async-mpi
    PROPERTIES
      SKIP_RETURN_CODE 125)
  catalyst_test_use_stub_backend(catalyst-async-mpi)
endif ()


if(CATALYST_WRAP_PYTHON)
  catalyst_add_python_test(SCRIPT catalyst_conduit/test_conduit_import.py
                           NAME  test_conduit_import_python)

  catalyst_add_python_test(SCRIPT test_catalyst_import.py
                           NAME test_catalyst_import_python)

  catalyst_add_python_test(SCRIPT test_double_impl.py
                           NAME test_double_impl_python)
endif()

if(CATALYST_WRAP_FORTRAN)
  add_executable(test_double_impl_f test_double_impl.f90 )
  target_link_libraries(test_double_impl_f catalyst::catalyst_fortran)
  add_test(NAME catalyst-impl-double-fortran COMMAND test_double_impl_f)
endif()
