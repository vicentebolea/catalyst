add_library(catalyst_fortran
  catalyst_api.f90)

add_library(catalyst::catalyst_fortran ALIAS catalyst_fortran)

target_link_libraries(catalyst_fortran
  PUBLIC
    catalyst::catalyst)

target_link_libraries(catalyst_fortran
  PUBLIC
    catalyst::catalyst)

if (CATALYST_WITH_EXTERNAL_CONDUIT)
  # link catalyst::catalyst to the catalyst_conduit namespace for fortran API
  add_library(catalyst_conduit_fortran OBJECT
     catalyst_conduit_external_api.f90)

  target_link_libraries(catalyst_conduit_fortran
    PRIVATE
      conduit::conduit)

  target_link_libraries(catalyst
    PUBLIC
      catalyst_conduit_fortran)
endif()

# set include directories for fortran module `.mod` files
target_include_directories(catalyst_fortran PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CATALYST_INSTALL_INCLUDE_DIR}>)

if (NOT CATALYST_WITH_EXTERNAL_CONDUIT)
  target_include_directories(catalyst_fortran PUBLIC
    $<INSTALL_INTERFACE:${CATALYST_INSTALL_INCLUDE_DIR}/conduit>)
endif()


set(catalyst_fortran_modules
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>/catalyst_api.mod)

if (CATALYST_WITH_EXTERNAL_CONDUIT)
  list(APPEND catalyst_fortran_modules
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>/catalyst_conduit.mod)
endif ()

set_property(TARGET catalyst_fortran APPEND
  PROPERTY EXPORT_PROPERTIES
    CATALYST_WITH_EXTERNAL_CONDUIT)

c_install_targets(catalyst_fortran)
if (CATALYST_WITH_EXTERNAL_CONDUIT)
  c_install_targets(catalyst_conduit_fortran)
endif()
c_install_headers(
  HEADERS ${catalyst_fortran_modules})
