set(libb64_sources
    src/cdecode.cpp
    src/cencode.cpp
    )

set(libb64_headers
    include/b64/cdecode.h
    include/b64/cencode.h
    include/b64/decode.h
    include/b64/encode.h
    include/b64/catalyst_b64_mangle.h)

add_library(catalyst_conduit_b64 OBJECT
  ${libb64_sources})
target_include_directories(catalyst_conduit_b64
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
set_property(TARGET catalyst_conduit_b64 PROPERTY
  EXPORT_NAME conduit_b64)
add_library(catalyst::conduit_b64 ALIAS catalyst_conduit_b64)
