include(catalyst-python-macros)
set(catalyst_python_cpp_sources catalyst_python.cpp)
set(catalyst_python_py_sources catalyst/__init__.py)

#-------------------------------------------------------------------------

Python3_add_library(catalyst_python ${catalyst_python_cpp_sources})

if (CATALYST_WITH_EXTERNAL_CONDUIT)
  target_link_libraries(catalyst_python
    PRIVATE 
      catalyst::catalyst 
      conduit::conduit
      conduit::conduit_python
      catalyst_python_flags
  )
else()
  target_link_libraries(catalyst_python
    PRIVATE 
      catalyst::catalyst 
      catalyst::conduit 
      catalyst::conduit_b64
      catalyst::conduit_libyaml
      catalyst::conduit_yyjson
      catalyst::conduit_python_cpp_headers 
      catalyst_python_flags
  )
endif()

set(CATALYST_PYTHON_SITE_PACKAGES_SUFFIX)
catalyst_module_python_default_destination(CATALYST_PYTHON_SITE_PACKAGES_SUFFIX)

configure_file(
  catalyst/__init__.py.in
  catalyst/__init__.py
   @ONLY)

set(catalyst_python_sources
  "${CMAKE_CURRENT_BINARY_DIR}/catalyst/__init__.py"
)

catalyst_install_python_library(catalyst_python
   MODULE_DESTINATION catalyst)
  
catalyst_module_add_python_package(catalyst
   PACKAGE catalyst
   FILES ${catalyst_python_sources})

if (CATALYST_WITH_EXTERNAL_CONDUIT)

  set(conduit_py_python_sources
      # conduit
      catalyst_conduit/__init__.py
      catalyst_conduit/utils/__init__.py
      # conduit.blueprint
      catalyst_conduit/blueprint/__init__.py
      catalyst_conduit/blueprint/mcarray/__init__.py
      catalyst_conduit/blueprint/mesh/__init__.py
      catalyst_conduit/blueprint/table/__init__.py)

  catalyst_module_add_python_package(catalyst_conduit
    PACKAGE catalyst_conduit
    FILES ${conduit_py_python_sources})

endif()

# make possible for the installed module to find libcatalyst during runtime
if (UNIX)
    file(RELATIVE_PATH _catalyst_python_relpath
      "${CMAKE_BINARY_DIR}/${CATALYST_PYTHON_SITE_PACKAGES_SUFFIX}"
      "${CMAKE_BINARY_DIR}")

    if (APPLE)
      set(_catalyst_python_origin_stem "@loader_path")
    else ()
      set(_catalyst_python_origin_stem "$ORIGIN")
    endif()

    set_target_properties(catalyst_python PROPERTIES INSTALL_RPATH "${_catalyst_python_origin_stem}/${_catalyst_python_relpath}")
endif ()
