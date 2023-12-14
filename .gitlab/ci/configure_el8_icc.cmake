 # silence remark about classic compiler getting deprecated
set(CMAKE_C_FLAGS "-diag-disable=10441" CACHE STRING "")
set(CMAKE_CXX_FLAGS "-diag-disable=10441" CACHE STRING "")

include("${CMAKE_CURRENT_LIST_DIR}/configure_common.cmake")
