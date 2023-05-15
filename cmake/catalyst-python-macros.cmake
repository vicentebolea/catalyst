# Derived from VTK's VTK/CMake/vtkModuleWrapPython.cmake
#[==[
@brief Determine Python module destination

Some projects may need to know where Python expects its modules to be placed in
the install tree (assuming a shared prefix). This function computes the default
and sets the passed variable to the value in the calling scope.

~~~
catalyst_module_python_default_destination(<var>)
~~~

By default, the destination is `${CMAKE_INSTALL_BINDIR}/Lib/site-packages` on
Windows and `${CMAKE_INSTALL_LIBDIR}/python<VERSION>/site-packages` otherwise.
#]==]

function (catalyst_module_python_default_destination var)
  if (_catalyst_module_python_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR
      "Unparsed arguments for catalyst_module_python_default_destination: "
      "${_catalyst_module_python_UNPARSED_ARGUMENTS}")
  endif ()

  if (MSVC)
    set(destination "${CMAKE_INSTALL_BINDIR}/Lib/site-packages")
  else ()
    if (NOT DEFINED "Python3_VERSION_MAJOR" OR
        NOT DEFINED "Python3_VERSION_MINOR")
      find_package("Python3" QUIET COMPONENTS Development.Module)
    endif ()

    if (Python3_VERSION_MAJOR AND Python3_VERSION_MINOR)
      set(_catalyst_python_version_suffix "${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}")
    else ()
      message(WARNING
        "The version of Python is unknown; not using a versioned directory "
        "for Python modules.")
      set(_catalyst_python_version_suffix)
    endif ()
    set(destination "${CMAKE_INSTALL_LIBDIR}/python${_catalyst_python_version_suffix}/site-packages")
  endif ()

  set("${var}" "${destination}" PARENT_SCOPE)
endfunction ()


#[==[
@brief Install Python library to specified path

Set the path for both the build and the install tree for the provided library.
It uses the project's basepath as evaluated in catalyst_module_python_default_destination
and combines it with the provided MODULE_DESTINATION which should be given relative to
catalyst_module_python_default_destination.

~~~
catalyst_install_python_library(<taget_name>
  MODULE_DESTINATION <destination>
)
~~~

#]==]
function (catalyst_install_python_library _name)

  cmake_parse_arguments(PARSE_ARGV 1 _catalyst_install_python_library
    ""
    "MODULE_DESTINATION"
    "")

  if (_catalyst_install_python_library_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR
      "Unparsed arguments for catalyst_module_install_python_library: "
      "${_catalyst_install_python_library_UNPARSED_ARGUMENTS}")
  endif ()

  if (NOT _catalyst_install_python_library_MODULE_DESTINATION)
    message(FATAL_ERROR
      "The `MODULE_DESTINATION` argument is required.")
  endif ()

  set(_catalyst_install_python_library_basepath)
  catalyst_module_python_default_destination(_catalyst_install_python_library_basepath)
  install(
    TARGETS ${_name}
    EXPORT  Catalyst
    DESTINATION "${_catalyst_install_python_library_basepath}/${_catalyst_install_python_library_MODULE_DESTINATION}"
   )

    set(_catalyst_binary_dir
      "${CMAKE_BINARY_DIR}/${_catalyst_install_python_library_basepath}/${_catalyst_install_python_library_MODULE_DESTINATION}")

  set_target_properties(${_name} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${_catalyst_binary_dir}"
      LIBRARY_OUTPUT_DIRECTORY "${_catalyst_binary_dir}")
endfunction ()


#[==[
@brief Install Python packages.

~~~
catalyst_add_python_package(<module>
  PACKAGE <package>
  FILES <files>...
  [MODULE_DESTINATION <destination>]
~~~

  * `PACKAGE`: (Required) The package installed by this call.
  * `FILES`: (Required) File paths should be relative to the source directory
    of the calling `CMakeLists.txt`. Upward paths are not supported (nor are
    checked for). Absolute paths are assumed to be in the build tree and their
    relative path is computed relative to the current binary directory.

A `<package>-files` target is created which ensures that all Python modules
have been copied to the correct location in the build tree.

#]==]
function (catalyst_module_add_python_package name)
  set(_catalyst_build_module ${name})
  if (NOT name STREQUAL _catalyst_build_module)
    message(FATAL_ERROR
      "Python modules must match their module names.")
  endif ()

  cmake_parse_arguments(PARSE_ARGV 1 _catalyst_add_python_package
    ""
    "PACKAGE"
    "FILES")

  if (_catalyst_add_python_package_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR
      "Unparsed arguments for catalyst_module_add_python_package: "
      "${_catalyst_add_python_package_UNPARSED_ARGUMENTS}")
  endif ()

  if (NOT _catalyst_add_python_package_PACKAGE)
    message(FATAL_ERROR
      "The `PACKAGE` argument is required.")
  endif ()
  string(REPLACE "." "/" _catalyst_add_python_package_path "${_catalyst_add_python_package_PACKAGE}")

  if (NOT _catalyst_add_python_package_FILES)
    message(FATAL_ERROR
      "The `FILES` argument is required.")
  endif ()

  # initialize path
  set(_catalyst_add_python_package_MODULE_DESTINATION)
  catalyst_module_python_default_destination(_catalyst_add_python_package_MODULE_DESTINATION)

  set(_catalyst_add_python_package_file_outputs)
  foreach (_catalyst_add_python_package_file IN LISTS _catalyst_add_python_package_FILES)
    if (IS_ABSOLUTE "${_catalyst_add_python_package_file}")
      file(RELATIVE_PATH _catalyst_add_python_package_name
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${_catalyst_add_python_package_file}")
    else ()
      set(_catalyst_add_python_package_name
        "${_catalyst_add_python_package_file}")
      string(PREPEND _catalyst_add_python_package_file
        "${CMAKE_CURRENT_SOURCE_DIR}/")
    endif ()

   # calculate full path in binary tree
    set(_catalyst_add_python_package_file_output
      "${CMAKE_BINARY_DIR}/${_catalyst_add_python_package_MODULE_DESTINATION}/${_catalyst_add_python_package_name}")
    # copy files in build tree
    add_custom_command(
      OUTPUT  "${_catalyst_add_python_package_file_output}"
      DEPENDS "${_catalyst_add_python_package_file}"
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
              "${_catalyst_add_python_package_file}"
              "${_catalyst_add_python_package_file_output}"
      COMMENT "Copying ${_catalyst_add_python_package_name} to the binary directory")

    list(APPEND _catalyst_add_python_package_file_outputs
      "${_catalyst_add_python_package_file_output}")

    # set path for install tree
    if (BUILD_SHARED_LIBS)
      get_filename_component(_catalyst_add_python_package_install_path "${_catalyst_add_python_package_name}" DIRECTORY)
      install(
        FILES       "${_catalyst_add_python_package_name}"
        DESTINATION "${_catalyst_add_python_package_MODULE_DESTINATION}/${_catalyst_add_python_package_install_path}")
    endif()
  endforeach ()

  # create a target that depends on all files to make sure they are copied in the build tree each time they are modified
  add_custom_target("${_catalyst_add_python_package_PACKAGE}-files" ALL
    DEPENDS
      ${_catalyst_add_python_package_file_outputs})
endfunction ()


#[==[
@brief Add a python test

~~~
catalyst_add_python_test(
  [SCRIPT <path to script>
  [NAME <test_name>]
)
~~~
  * `SCRIPT`: (Required) path to the test script
  * `NAME`: (Optional) name for this test. If none is provide `<test>` will be used.

Creates a cmake test with the PYTHONPATH environmental variable set  so that
python can find our modules
#]==]
function (catalyst_add_python_test)
  cmake_parse_arguments(PARSE_ARGV 0 _catalyst_add_python_test
    ""
    "SCRIPT;NAME"
    "")

  if (_catalyst_add_python_test_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR
      "Unparsed arguments for catalyst_add_test: "
      "${_catalyst_add_python_test_UNPARSED_ARGUMENTS}")
  endif ()

  if (NOT _catalyst_add_python_test_SCRIPT)
    message(FATAL_ERROR
      "The SCRIPT argument is required")
  endif ()

  if (NOT _catalyst_add_python_test_NAME)
     set(_catalyst_add_python_test_NAME ${_catalyst_add_python_test_SCRIPT})
  endif ()

  add_test(NAME ${_catalyst_add_python_test_NAME}
      COMMAND ${CMAKE_COMMAND} -E env --modify PYTHONPATH=path_list_prepend:${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages
          ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${_catalyst_add_python_test_SCRIPT}
  )
  set_tests_properties(${_catalyst_add_python_test_NAME} PROPERTIES LABELS "Python")
endfunction()
