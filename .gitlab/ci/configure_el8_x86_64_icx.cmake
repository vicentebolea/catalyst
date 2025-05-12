# silence remark about optimization being disabled due to compiling in debug mode
set(CMAKE_C_FLAGS "-Rno-debug-disables-optimization" CACHE STRING "")
set(CMAKE_CXX_FLAGS "-Rno-debug-disables-optimization" CACHE STRING "")
include("${CMAKE_CURRENT_LIST_DIR}/configure_common.cmake")
