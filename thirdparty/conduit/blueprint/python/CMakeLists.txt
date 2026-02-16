configure_file("${CMAKE_CURRENT_SOURCE_DIR}/conduit_blueprint_python_exports.h.in"
                "${CMAKE_CURRENT_BINARY_DIR}/conduit_blueprint_python_exports.h")
 
#-------------------------------------------------------------------------
# conduit.blueprint module

Python3_add_library(conduit_blueprint_python conduit_blueprint_python.cpp)
add_library(catalyst::conduit_blueprint_python ALIAS conduit_blueprint_python)

target_include_directories(conduit_blueprint_python
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

target_link_libraries(conduit_blueprint_python
  PRIVATE 
    catalyst::blueprint
    catalyst::blueprint_cpp_headers
    catalyst::conduit 
    catalyst::conduit_b64
    catalyst::conduit_libyaml
    catalyst::conduit_yyjson
    catalyst::conduit_python_cpp_headers
    conduit_python_flags)

catalyst_install_python_library(conduit_blueprint_python
  MODULE_DESTINATION catalyst_conduit/blueprint)

#-------------------------------------------------------------------------
# conduit.blueprint.mcarray module 

Python3_add_library(conduit_blueprint_mcarray_python conduit_blueprint_mcarray_python.cpp)
add_library(catalyst::conduit_blueprint_mcarray_python ALIAS conduit_blueprint_mcarray_python)

target_include_directories(conduit_blueprint_mcarray_python
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

target_link_libraries(conduit_blueprint_mcarray_python
  PRIVATE 
    catalyst::blueprint
    catalyst::blueprint_cpp_headers
    catalyst::conduit 
    catalyst::conduit_b64
    catalyst::conduit_libyaml
    catalyst::conduit_yyjson
    catalyst::conduit_python_cpp_headers
    conduit_python_flags)

catalyst_install_python_library(conduit_blueprint_mcarray_python
  MODULE_DESTINATION catalyst_conduit/blueprint/mcarray)
# we skip blueprint_mcarray_examples since we not provide the cpp sources


#-------------------------------------------------------------------------
# conduit.blueprint.mesh module

Python3_add_library(conduit_blueprint_mesh_python conduit_blueprint_mesh_python.cpp)
add_library(catalyst::conduit_blueprint_mesh_python ALIAS conduit_blueprint_mesh_python)

target_include_directories(conduit_blueprint_mesh_python
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

target_link_libraries(conduit_blueprint_mesh_python
  PRIVATE 
    catalyst::blueprint
    catalyst::blueprint_cpp_headers
    catalyst::conduit 
    catalyst::conduit_b64
    catalyst::conduit_libyaml
    catalyst::conduit_yyjson
    catalyst::conduit_python_cpp_headers
    conduit_python_flags)

catalyst_install_python_library(conduit_blueprint_mesh_python
  MODULE_DESTINATION catalyst_conduit/blueprint/mesh)

#-------------------------------------------------------------------------
# conduit.blueprint.table module

Python3_add_library(conduit_blueprint_table_python conduit_blueprint_table_python.cpp)
add_library(catalyst::conduit_blueprint_table_python ALIAS conduit_blueprint_table_python)

target_include_directories(conduit_blueprint_table_python
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

target_link_libraries(conduit_blueprint_table_python
  PRIVATE 
    catalyst::blueprint
    catalyst::blueprint_cpp_headers
    catalyst::conduit 
    catalyst::conduit_b64
    catalyst::conduit_libyaml
    catalyst::conduit_yyjson
    catalyst::conduit_python_cpp_headers
    conduit_python_flags)

catalyst_install_python_library(conduit_blueprint_table_python
  MODULE_DESTINATION catalyst_conduit/blueprint/table)
