cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
project(CATALYST VERSION 2.0 LANGUAGES C CXX)

# De-duplicate libraries on link lines based on linker capabilities.
if (POLICY CMP0156)
  cmake_policy(SET CMP0156 NEW)
endif ()

include(CMakeDependentOption)

#------------------------------------------------------------------------------
# Options affecting the build
# These should be kept to a minimum.
# Note, due to CMP0077, we no longer need to check if option-variable is
# already defined.
option(CATALYST_BUILD_SHARED_LIBS "Enable shared libs" ON)
option(CATALYST_BUILD_TESTING "Enable testing" ON)
option(CATALYST_BUILD_TOOLS "Build the catalyst tools" ON)
option(CATALYST_USE_MPI "Enable MPI related features" OFF)
option(CATALYST_WRAP_PYTHON "Build Python FFI bindings to libcatalyst" OFF)
if (CATALYST_WRAP_PYTHON)
  find_package(Python3 COMPONENTS Development NumPy REQUIRED)
endif ()
option(CATALYST_WRAP_FORTRAN "Build Fortran FFI bindings to libcatalyst" OFF)
if (CATALYST_WRAP_FORTRAN)
  enable_language(Fortran)
endif ()

#------------------------------------------------------------------------------
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(catalyst-macros)
include(catalyst-internal-macros)
include(catalyst-sanitize)
include(cCompilerChecks)
include(cInitializeBuildType)
include(cDirectories)
if (CATALYST_WRAP_PYTHON)
  include(catalyst-python-macros)
  include(catalyst-python-flags)
endif ()

# Currently, we require shared builds.
set(BUILD_SHARED_LIBS ${CATALYST_BUILD_SHARED_LIBS})

# `CATALYST_ABI_VERSION` is the version of the ABI. This should
# only be the major version number since minor version number changes
# should not affect ABI at all.
set(CATALYST_ABI_VERSION "${CATALYST_VERSION_MAJOR}")

# `CATALYST_CUSTOM_LIBRARY_SUFFIX` can be used to override the suffix used
# for static libraries built by this project. Note this does not affect the
# Catalyst library itself, but all the internal dependencies.
c_set_if_not_set(CATALYST_CUSTOM_LIBRARY_SUFFIX "_catalyst${CATALYST_VERSION}")

if(CATALYST_USE_MPI)
  find_package(MPI COMPONENTS C REQUIRED)
endif()

# Some default settings.
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_C_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

option(CATALYST_WITH_EXTERNAL_CONDUIT "Build Catalyst with an external Conduit library" OFF)
mark_as_advanced(CATALYST_WITH_EXTERNAL_CONDUIT)

if (CATALYST_WITH_EXTERNAL_CONDUIT)
  # if version changes, make sure to update all places where "conduit_version" is used
  find_package(Conduit 0.8.7 REQUIRED)
  if(CATALYST_WRAP_FORTRAN AND (NOT CONDUIT_FORTRAN_ENABLED))
      message(FATAL_ERROR "Provided Conduit library was NOT built with Fortran support.")
  endif()
  
  if(CATALYST_WRAP_PYTHON AND (NOT CONDUIT_PYTHON_ENABLED))
      message(FATAL_ERROR "Provided Conduit library was NOT built with Python support.")
  endif()
endif()

if (NOT CATALYST_WITH_EXTERNAL_CONDUIT)
  add_subdirectory(thirdparty/conduit)
endif ()

add_subdirectory(src)

# include install/package rules.
include(cInstallCMakePackage)

#------------------------------------------------------------------------------
# Testing, we build:
# 1.) examples and their tests
# 2.) Some conduit-native tests to test our wrapped C API
# 3.) catalyst_replay tests, if enabled
# That's our testing suite.
if (CATALYST_BUILD_TESTING)
  set(BUILD_TESTING ON)
  include(CTest)
  include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/catalyst-test-macros.cmake")
  add_install_test()
  add_subdirectory(examples)

  add_subdirectory(tests)
endif()
#------------------------------------------------------------------------------
