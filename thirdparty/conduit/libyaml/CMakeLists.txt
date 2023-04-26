set(libyaml_sources
    src/api.c
    src/dumper.c
    src/emitter.c
    src/loader.c
    src/parser.c
    src/reader.c
    src/scanner.c
    src/writer.c
    )

#
# libyaml headers
#

set(libyaml_headers
    include/yaml.h
    include/catalyst_yaml_mangle.h
    src/yaml_private.h)

add_library(catalyst_conduit_libyaml OBJECT
  ${libyaml_sources})
target_include_directories(catalyst_conduit_libyaml
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
set_property(TARGET catalyst_conduit_libyaml PROPERTY
  EXPORT_NAME conduit_libyaml)
add_library(catalyst::conduit_libyaml ALIAS catalyst_conduit_libyaml)

if (WIN32)
  target_compile_definitions(catalyst_conduit_libyaml
    PRIVATE
      CONDUIT_PLATFORM_WINDOWS)
endif()

# use cxx linker to avoid msvc static build issues
set_target_properties(catalyst_conduit_libyaml PROPERTIES LINKER_LANGUAGE CXX)
