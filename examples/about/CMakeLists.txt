cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
project(CATALYST_EXPORT_ABOUT)

find_package(catalyst REQUIRED)

add_executable(catalyst_example_about
  catalyst_example_about.cxx)

target_compile_features(catalyst_example_about
  PRIVATE cxx_std_11)
target_link_libraries(catalyst_example_about
  PRIVATE catalyst::catalyst)

include(CTest)
if (BUILD_TESTING)
  add_test(
    NAME test
    COMMAND catalyst_example_about --help)
endif()
