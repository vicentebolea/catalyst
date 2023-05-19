add_library(catalyst_fortran
  catalyst_api.f90)

add_library(catalyst::catalyst_fortran ALIAS catalyst_fortran)
target_link_libraries(catalyst_fortran
  PUBLIC
    catalyst::catalyst)

# set include directories for fortran module `.mod` files
target_include_directories(catalyst_fortran PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CATALYST_INSTALL_INCLUDE_DIR}> 
  $<INSTALL_INTERFACE:${CATALYST_INSTALL_INCLUDE_DIR}/conduit>)


set(catalyst_fortran_modules 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>/catalyst_api.mod)

c_install_targets(catalyst_fortran)
c_install_headers(
  HEADERS ${catalyst_fortran_modules})
