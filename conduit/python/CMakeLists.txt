configure_file("${CMAKE_CURRENT_SOURCE_DIR}/conduit_python_exports.h.in"
               "${CMAKE_CURRENT_BINARY_DIR}/conduit_python_exports.h")

# Specify the sources of the pure python and compiled portions of our module.
set(conduit_py_python_sources
    # conduit
    catalyst_conduit/__init__.py
    catalyst_conduit/utils/__init__.py
    # conduit.blueprint
    catalyst_conduit/blueprint/__init__.py
    catalyst_conduit/blueprint/mcarray/__init__.py
    catalyst_conduit/blueprint/mesh/__init__.py
    catalyst_conduit/blueprint/table/__init__.py)


catalyst_module_add_python_package(catalyst_conduit 
  PACKAGE catalyst_conduit
  FILES ${conduit_py_python_sources})

set(conduit_py_headers 
    conduit_python.hpp
    ${CMAKE_CURRENT_BINARY_DIR}/conduit_python_exports.h)

set(conduit_py_cpp_sources 
    conduit_python.cpp)

#-------------------------------------------------------------------------
# interface library to hold platform flags
set(_platform_flags)

# Xcode 13.0+ automatically enables fixup_chains which introduces 
# "ld: warning: -undefined dynamic_lookup may not work with chained fixups" warnings
# Apple Clang >= 14.0.3 (Xcode 14.3) automatically disables fixup_chains  when
# dynamic_lookup is used (default for python libraries)
if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND
     CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 13.0 AND
     CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14.0.3)
set(_platform_flags
    "LINKER:-no_fixup_chains")
endif ()

add_library(conduit_python_flags INTERFACE)
target_link_options(conduit_python_flags
  INTERFACE
     ${_platform_flags})

#-------------------------------------------------------------------------
# conduit module

Python3_add_library(conduit_python ${conduit_py_cpp_sources})
add_library(catalyst::conduit_python ALIAS conduit_python)

target_link_libraries(conduit_python
  PRIVATE 
    Python3::NumPy 
    catalyst::conduit 
    catalyst::conduit_fmt
    catalyst::conduit_libyaml
    catalyst::conduit_b64
    catalyst::conduit_rapidjson
    conduit_python_flags
    )

target_include_directories(conduit_python
  PRIVATE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)


add_library(catalyst_conduit_python_cpp_headers INTERFACE)
add_library(catalyst::conduit_python_cpp_headers ALIAS catalyst_conduit_python_cpp_headers)

target_include_directories(catalyst_conduit_python_cpp_headers 
  INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

catalyst_install_python_library(conduit_python
  MODULE_DESTINATION catalyst_conduit)


#-------------------------------------------------------------------------
# conduit.utils module

Python3_add_library(conduit_utils_python conduit_utils_python.cpp)
add_library(catalyst::conduit_utils_python ALIAS conduit_utils_python)

target_link_libraries(conduit_utils_python
  PRIVATE 
    catalyst::conduit 
    catalyst::conduit_b64
    catalyst::conduit_libyaml
    conduit_python_flags)

target_include_directories(conduit_utils_python
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

catalyst_install_python_library(conduit_utils_python
  MODULE_DESTINATION catalyst_conduit/utils)
