set(headers
  catalyst.h
  catalyst_api.h
  catalyst_impl.h
  catalyst_stub.h
  catalyst.hpp
  catalyst_conduit.hpp
  catalyst_conduit_blueprint.hpp
  catalyst_conduit_error.hpp
  catalyst_python_tools.h
  ${CMAKE_CURRENT_BINARY_DIR}/catalyst_conduit.h
  ${CMAKE_CURRENT_BINARY_DIR}/catalyst_conduit_abi.h
  ${CMAKE_CURRENT_BINARY_DIR}/catalyst_export.h
  ${CMAKE_CURRENT_BINARY_DIR}/catalyst_version.h)
configure_file(catalyst_version.h.in catalyst_version.h)

if (CATALYST_WITH_EXTERNAL_CONDUIT)
  set(catalyst_conduit_state external)
else ()
  set(catalyst_conduit_state internal)
endif ()

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/catalyst_conduit_abi_${catalyst_conduit_state}.h"
  "${CMAKE_CURRENT_BINARY_DIR}/catalyst_conduit_abi.h"
  COPYONLY)
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/catalyst_conduit_${catalyst_conduit_state}.h"
  "${CMAKE_CURRENT_BINARY_DIR}/catalyst_conduit.h"
  COPYONLY)

set(private_headers
  catalyst_dump_node.hpp)

#------------------------------------------------------------
# Add an interface library just to setup include dirs.
add_library(catalyst_headers INTERFACE)
target_include_directories(catalyst_headers
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CATALYST_INSTALL_INCLUDE_DIR}>)
add_library(catalyst::catalyst_headers ALIAS catalyst_headers)

if (WIN32)
  set_property(SOURCE catalyst_api.c APPEND
    PROPERTY
      COMPILE_DEFINITIONS "CATALYST_DEBUG_SUFFIX=\"$<$<CONFIG:Debug>:d>\"")
endif ()

add_library(catalyst
  catalyst_api.c
  catalyst_api_default.cpp
  catalyst_stub.cpp
  catalyst_conduit_abi_${catalyst_conduit_state}.c
  catalyst_python_tools.cpp
  ${headers}
  ${private_headers})
target_compile_features(catalyst
  PRIVATE
    c_std_99)
set_target_properties(catalyst
  PROPERTIES
    # This should never need to be bumped. libcatalyst is intended to be stable
    # in its ABI, so removing APIs is not something we should be doing. This
    # includes exposed Conduit APIs.
    SOVERSION 3)
target_link_libraries(catalyst
  PUBLIC
    catalyst::catalyst_headers
  PRIVATE
    ${CMAKE_DL_LIBS})

set(conduit_public_libraries)
set(conduit_private_libraries)
if (CATALYST_WITH_EXTERNAL_CONDUIT)
  set(conduit_public_libraries
      conduit::conduit)
  if(CATALYST_WRAP_PYTHON)
    set(conduit_private_libraries 
       conduit::conduit_python
       Python3::Python)
  endif()
else ()
  set(conduit_public_libraries
      "$<BUILD_INTERFACE:catalyst::blueprint_headers>"
      "$<BUILD_INTERFACE:catalyst::conduit_headers>")
  set(conduit_private_libraries
      "$<BUILD_LOCAL_INTERFACE:catalyst::conduit>"
      "$<BUILD_LOCAL_INTERFACE:catalyst::blueprint>"
      "$<BUILD_LOCAL_INTERFACE:catalyst::conduit_libyaml>"
      "$<BUILD_LOCAL_INTERFACE:catalyst::conduit_b64>")
  if(CATALYST_WRAP_PYTHON)
    set(conduit_private_libraries 
        ${conduit_private_libraries}
       catalyst::conduit_python_cpp_headers 
       Python3::Python)
  endif()
endif ()

target_link_libraries(catalyst
  PUBLIC
    ${conduit_public_libraries}
  PRIVATE
    ${conduit_private_libraries})
set(catalyst_conduit_is_external 0)
if (CATALYST_WITH_EXTERNAL_CONDUIT)
  set(catalyst_conduit_is_external 1)
endif ()
target_compile_definitions(catalyst
  PUBLIC
    "CATALYST_WRAP_PYTHON=$<BOOL:${CATALYST_WRAP_PYTHON}>"
  PRIVATE
    "CATALYST_WITH_EXTERNAL_CONDUIT=${catalyst_conduit_is_external}")
set_property(TARGET catalyst
  PROPERTY CATALYST_WITH_EXTERNAL_CONDUIT
    "${catalyst_conduit_is_external}")
set_property(TARGET catalyst APPEND
  PROPERTY EXPORT_PROPERTIES
    CATALYST_WITH_EXTERNAL_CONDUIT)

if (WIN32)
  set_property(TARGET catalyst
    PROPERTY
      DEBUG_POSTFIX d)
endif ()

if (BUILD_SHARED_LIBS)
  target_compile_definitions(catalyst
    PRIVATE
      conduit_c_EXPORTS
      conduit_blueprint_c_EXPORTS)
endif ()

target_compile_features(catalyst
  PUBLIC
    cxx_std_11)

add_library(catalyst::catalyst ALIAS catalyst)

if (CATALYST_USE_MPI)
  target_link_libraries(catalyst PRIVATE MPI::MPI_C)
  target_compile_definitions(catalyst
    PRIVATE CATALYST_USE_MPI
            # force `mpi.h` to not include cxx components to avoid
            # unnecessary dependency on MPI_CPP
            MPICH_SKIP_MPICXX
            OMPI_SKIP_MPICXX
            MPI_NO_CPPBIND
            _MPICC_H)
endif ()

c_install_targets(catalyst catalyst_headers)
c_install_headers(
  HEADERS ${headers})

#-------------------------------------------------------------
# Generate export header.
set_target_properties(catalyst
  PROPERTIES
    DEFINE_SYMBOL catalyst_EXPORTS)
include(GenerateExportHeader)
generate_export_header(catalyst)
