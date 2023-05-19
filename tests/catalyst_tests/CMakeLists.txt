add_subdirectory(impls)

add_executable(test_double_impl test_double_impl.c)
target_link_libraries(test_double_impl catalyst::catalyst)
add_test(NAME catalyst-impl-double COMMAND test_double_impl)

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
