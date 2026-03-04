add_subdirectory(impls)

add_executable(test_double_impl test_double_impl.c)
target_link_libraries(test_double_impl catalyst::catalyst)
add_test(NAME catalyst-impl-double COMMAND test_double_impl)

add_executable(test_about_impl_path test_about_impl_path.c)
target_link_libraries(test_about_impl_path catalyst::catalyst)
add_test(NAME catalyst-about-impl-path COMMAND test_about_impl_path)
# Unset implementation env vars to ensure stub backend is used
set_property(TEST catalyst-about-impl-path APPEND PROPERTY
  ENVIRONMENT_MODIFICATION "CATALYST_IMPLEMENTATION_PATHS=unset:" "CATALYST_IMPLEMENTATION_NAME=unset:")

add_executable(test_about_impl_path_double test_about_impl_path_double.c)
target_link_libraries(test_about_impl_path_double catalyst::catalyst)
add_test(NAME catalyst-about-impl-path-double COMMAND test_about_impl_path_double "$<TARGET_FILE:catalyst-double>")
# Unset implementation env vars so the test controls which impl is loaded
set_property(TEST catalyst-about-impl-path-double APPEND PROPERTY
  ENVIRONMENT_MODIFICATION "CATALYST_IMPLEMENTATION_PATHS=unset:" "CATALYST_IMPLEMENTATION_NAME=unset:")

add_executable(test_about_impl_path_params test_about_impl_path_params.c)
target_link_libraries(test_about_impl_path_params catalyst::catalyst)
add_test(NAME catalyst-about-impl-path-params COMMAND test_about_impl_path_params "$<TARGET_FILE_DIR:catalyst-double>")
# Unset implementation env vars so the test controls which impl is loaded
set_property(TEST catalyst-about-impl-path-params APPEND PROPERTY
  ENVIRONMENT_MODIFICATION "CATALYST_IMPLEMENTATION_PATHS=unset:" "CATALYST_IMPLEMENTATION_NAME=unset:")

add_executable(test_about_impl_path_env test_about_impl_path_env.c)
target_link_libraries(test_about_impl_path_env catalyst::catalyst)
add_test(NAME catalyst-about-impl-path-env COMMAND test_about_impl_path_env "$<TARGET_FILE_DIR:catalyst-double>")
# Unset any existing env vars first - the test sets its own
set_property(TEST catalyst-about-impl-path-env APPEND PROPERTY
  ENVIRONMENT_MODIFICATION "CATALYST_IMPLEMENTATION_PATHS=unset:" "CATALYST_IMPLEMENTATION_NAME=unset:")

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
