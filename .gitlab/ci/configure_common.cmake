set(CATALYST_BUILD_TESTING             ON CACHE BOOL "")
set(CATALYST_BUILD_STUB_IMPLEMENTATION ON CACHE BOOL "")
set(CATALYST_BUILD_SHARED_LIBS         ON CACHE BOOL "")

set(CMAKE_PREFIX_PATH "$ENV{GIT_CLONE_PATH}/.gitlab/gtest"
  CACHE STRING "")

function (configuration_flag variable configuration)
  if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "${configuration}")
    set("${variable}" ON CACHE BOOL "")
  else ()
    set("${variable}" OFF CACHE BOOL "")
  endif ()
endfunction ()

configuration_flag(CATALYST_USE_MPI "mpi")
configuration_flag(CATALYST_BUILD_REPLAY "replay")

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "_tidy")
  set(CMAKE_C_CLANG_TIDY "clang-tidy" "--header-filter=$ENV{CI_PROJECT_DIR}/(src|tests)" CACHE FILEPATH "")
  set(CMAKE_CXX_CLANG_TIDY "clang-tidy" "--header-filter=$ENV{CI_PROJECT_DIR}/(src|tests)" CACHE FILEPATH "")
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "_asan")
  set(CATALYST_SANITIZER "address" CACHE STRING "")
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "_ubsan")
  set(CATALYST_SANITIZER "undefined" CACHE STRING "")
endif ()
