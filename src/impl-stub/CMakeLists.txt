if (APPLE)
  list(APPEND CMAKE_INSTALL_RPATH
    "@loader_path/..")
elseif (UNIX)
  list(APPEND CMAKE_INSTALL_RPATH
    "$ORIGIN/..")
endif ()

catalyst_implementation(
  TARGET  catalyst_stub
  NAME    stub
  EXPORT  Catalyst
  SOURCES catalyst.cxx)
