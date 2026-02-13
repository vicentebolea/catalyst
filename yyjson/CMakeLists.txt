set(yyjson_sources
    conduit_yyjson.cpp)

set(yyjson_headers
    conduit_yyjson_interface.h
    conduit_yyjson.h)

add_library(catalyst_conduit_yyjson OBJECT
  ${yyjson_sources})
target_include_directories(catalyst_conduit_yyjson
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
# Hide yyjson symbols - these are internal to conduit and should not pollute
# the Catalyst ABI. Define yyjson_api before the header's #ifndef check.
# Only use visibility attribute on non-Windows platforms.
if(NOT WIN32)
  target_compile_definitions(catalyst_conduit_yyjson PRIVATE
      "yyjson_api=__attribute__((visibility(\"hidden\")))")
endif()
set_property(TARGET catalyst_conduit_yyjson PROPERTY
  EXPORT_NAME conduit_yyjson)
add_library(catalyst::conduit_yyjson ALIAS catalyst_conduit_yyjson)
