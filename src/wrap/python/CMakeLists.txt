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

set(CATALYST_PYTHON_SITE_PACKAGES_SUFFIX)
catalyst_module_python_default_destination(CATALYST_PYTHON_SITE_PACKAGES_SUFFIX)

configure_file(
  catalyst/__init__.py.in
  "${CMAKE_BINARY_DIR}/${CATALYST_PYTHON_SITE_PACKAGES_SUFFIX}/catalyst/__init__.py"
   @ONLY)

catalyst_install_python_library(catalyst_python
   MODULE_DESTINATION catalyst)

# make possible for the installed module to find libcatalyst during runtime
set_target_properties(catalyst_python PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_LIBDIR}")
