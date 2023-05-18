include(catalyst-python-macros)
set(catalyst_python_cpp_sources catalyst_python.cpp)
set(catalyst_python_py_sources catalyst/__init__.py)


Python3_add_library(catalyst_python ${catalyst_python_cpp_sources})

target_link_libraries(catalyst_python
  PRIVATE 
    catalyst::catalyst 
    catalyst::conduit 
    catalyst::conduit_b64
    catalyst::conduit_libyaml
    catalyst::conduit_python_cpp_headers 
)

catalyst_install_python_library(catalyst_python
  MODULE_DESTINATION catalyst)

catalyst_module_add_python_package(catalyst
  PACKAGE catalyst
  FILES ${catalyst_python_py_sources})

# make possible for the installed module to find libcatalyst during runtime
set_target_properties(catalyst_python PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_LIBDIR}")
