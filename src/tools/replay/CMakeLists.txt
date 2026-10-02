if (APPLE)
   list(APPEND CMAKE_INSTALL_RPATH "@loader_path/../${CATALYST_INSTALL_LIBRARY_DIR}")
elseif (UNIX)
   list(APPEND CMAKE_INSTALL_RPATH "$ORIGIN/../${CATALYST_INSTALL_LIBRARY_DIR}")
endif ()


add_executable(catalyst_replay catalyst_replay.cpp)
target_link_libraries(catalyst_replay PRIVATE catalyst)
set_target_properties(catalyst_replay PROPERTIES
                                        BUILD_RPATH_USE_ORIGIN TRUE)

if(CATALYST_USE_MPI)
  target_link_libraries(catalyst_replay PRIVATE MPI::MPI_C)
  target_compile_definitions(catalyst_replay
    PRIVATE CATALYST_USE_MPI
            # force `mpi.h` to not include cxx components to avoid
            # unnecessary dependency on MPI_CPP
            MPICH_SKIP_MPICXX
            OMPI_SKIP_MPICXX
            MPI_NO_CPPBIND
            _MPICC_H)
endif()

c_install_targets(catalyst_replay)
