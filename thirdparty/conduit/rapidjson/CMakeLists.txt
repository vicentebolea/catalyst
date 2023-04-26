add_library(catalyst_conduit_rapidjson INTERFACE)
target_include_directories(catalyst_conduit_rapidjson
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
set_property(TARGET catalyst_conduit_rapidjson PROPERTY
  EXPORT_NAME conduit_rapidjson)
add_library(catalyst::conduit_rapidjson ALIAS catalyst_conduit_rapidjson)
