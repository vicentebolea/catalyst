#------------------------------------------------------------------------------
# Get build directory functional

set(_binary_package_dir "${CMAKE_BINARY_DIR}/${CATALYST_INSTALL_PACKAGE_DIR}")

set(catalyst_conduit_files)

include(CMakeDependentOption)
cmake_dependent_option(CATALYST_RELOCATABLE_INSTALL "If set, the install tree will not reference any paths outside the prefix" ON
  "CATALYST_WITH_EXTERNAL_CONDUIT" ON)

if (NOT CATALYST_WITH_EXTERNAL_CONDUIT)
  export(
    TARGETS
      catalyst_blueprint_headers
      catalyst_conduit_headers
    NAMESPACE
      catalyst::
    FILE
      ${_binary_package_dir}/catalyst-conduit-targets.cmake)
else ()
  set(non_relocatable_content
"if (NOT Conduit_DIR)
  set(Conduit_DIR \"${Conduit_DIR}\")
endif ()
")
  # if version changes, make sure to update all places where "conduit_version" is used
  set(relocatable_content
"include(CMakeFindDependencyMacro)
find_dependency(Conduit 0.8.7)
")
  file(WRITE "${_binary_package_dir}/catalyst-conduit-targets.cmake"
    "${non_relocatable_content}${relocatable_content}")
  if (CATALYST_RELOCATABLE_INSTALL)
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/catalyst-conduit-targets.cmake"
      "${relocatable_content}")
  else ()
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/catalyst-conduit-targets.cmake"
      "${non_relocatable_content}${relocatable_content}")
  endif ()
  list(APPEND catalyst_conduit_files
    "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/catalyst-conduit-targets.cmake")
endif ()

set(CATALYST_PYTHONPATH)
if (CATALYST_WRAP_PYTHON)
  set(path)
  catalyst_module_python_default_destination(path)
  set(paths ${path}
            ${CONDUIT_PYTHON_MODULE_DIR})
  if(WIN32)
    list(JOIN paths ";" CATALYST_PYTHONPATH)
  else()
    list(JOIN paths ":" CATALYST_PYTHONPATH)
  endif()
endif()
# These are targets for users of Catalyst
# i.e the simulation codes.
export(
  EXPORT    Catalyst
  NAMESPACE catalyst::
  FILE      ${_binary_package_dir}/catalyst-targets.cmake)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${_binary_package_dir}/catalyst-config-version.cmake"
  VERSION       "${CATALYST_VERSION}"
  COMPATIBILITY SameMajorVersion)

configure_package_config_file(
  "${CMAKE_CURRENT_LIST_DIR}/catalyst-config.cmake.in"
  "${_binary_package_dir}/catalyst-config.cmake"
  INSTALL_DESTINATION "${CATALYST_INSTALL_PACKAGE_DIR}")


configure_file("${CMAKE_CURRENT_LIST_DIR}/catalyst-macros.cmake"
               "${_binary_package_dir}/catalyst-macros.cmake" COPYONLY)
configure_file("${CMAKE_CURRENT_LIST_DIR}/catalyst_impl.c.in"
               "${_binary_package_dir}/catalyst_impl.c.in" COPYONLY)
configure_file("${CMAKE_CURRENT_LIST_DIR}/catalyst_impl.h.in"
               "${_binary_package_dir}/catalyst_impl.h.in" COPYONLY)
#------------------------------------------------------------------------------
# Get install directory functional

# We only install targes for users of Catalyst i.e. simulation codes
install(
  EXPORT      Catalyst
  NAMESPACE   catalyst::
  FILE        catalyst-targets.cmake
  DESTINATION "${CATALYST_INSTALL_PACKAGE_DIR}")

install(
  FILES       "${_binary_package_dir}/catalyst-config-version.cmake"
              "${_binary_package_dir}/catalyst-config.cmake"
              "${_binary_package_dir}/catalyst-macros.cmake"
              "${_binary_package_dir}/catalyst_impl.c.in"
              "${_binary_package_dir}/catalyst_impl.h.in"
              ${catalyst_conduit_files}
  DESTINATION "${CATALYST_INSTALL_PACKAGE_DIR}")
