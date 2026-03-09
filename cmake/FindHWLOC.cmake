#[=======================================================================[.rst:
FindHWLOC
---------

Find the hwloc library for portable hardware locality detection.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``HWLOC::HWLOC``
  The hwloc library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``HWLOC_FOUND``
  True if the system has the hwloc library.
``HWLOC_INCLUDE_DIRS``
  Include directories needed to use hwloc.
``HWLOC_LIBRARIES``
  Libraries needed to link to hwloc.

#]=======================================================================]

# Try pkg-config first
find_package(PkgConfig QUIET)
if (PkgConfig_FOUND)
  pkg_check_modules(_HWLOC QUIET hwloc)
endif ()

find_path(HWLOC_INCLUDE_DIR
  NAMES hwloc.h
  HINTS ${_HWLOC_INCLUDE_DIRS})

find_library(HWLOC_LIBRARY
  NAMES hwloc
  HINTS ${_HWLOC_LIBRARY_DIRS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HWLOC
  REQUIRED_VARS HWLOC_LIBRARY HWLOC_INCLUDE_DIR)

if (HWLOC_FOUND)
  set(HWLOC_LIBRARIES ${HWLOC_LIBRARY})
  set(HWLOC_INCLUDE_DIRS ${HWLOC_INCLUDE_DIR})

  if (NOT TARGET HWLOC::HWLOC)
    add_library(HWLOC::HWLOC UNKNOWN IMPORTED)
    set_target_properties(HWLOC::HWLOC PROPERTIES
      IMPORTED_LOCATION "${HWLOC_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${HWLOC_INCLUDE_DIR}")
  endif ()
endif ()

mark_as_advanced(HWLOC_INCLUDE_DIR HWLOC_LIBRARY)
