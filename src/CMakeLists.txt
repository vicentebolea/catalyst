add_subdirectory(catalyst)
add_subdirectory(wrap)

if (CATALYST_USE_MPI)
  add_subdirectory(mpi-shim)
endif ()

option(CATALYST_BUILD_STUB_IMPLEMENTATION "Enable stub implementation" ON)
if (CATALYST_BUILD_STUB_IMPLEMENTATION)
  add_subdirectory(impl-stub)
endif ()

if (CATALYST_BUILD_TOOLS)
  add_subdirectory(tools)
endif ()
