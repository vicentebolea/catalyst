!/*
! * Distributed under OSI-approved BSD 3-Clause License. See
! * accompanying License.txt
! */

! This driver is based on replay_high_num_execute_invc/high_num_execute_invc_driver.cxx

program high_num_execute_invc_driver

  use, intrinsic :: iso_c_binding, only: C_PTR
  use, intrinsic :: iso_fortran_env, only: stderr => error_unit
  use catalyst_api
  use catalyst_conduit
  implicit none

#ifdef CATALYST_USE_MPI
  include 'mpif.h'
#endif

  integer :: ierr
  type(C_PTR) node
  integer :: ret = 0
  integer(kind(catalyst_status)) :: err
  integer :: a = 10
  integer :: i = 0
  integer :: argc = 0
  character(len=4096) :: arg1

  argc = command_argument_count()

  if (argc < 1) then
    write (stderr, *) "ERROR: wrong argument count", argc
    call exit(1)
  end if

  call get_command_argument(1, arg1)

#ifdef CATALYST_USE_MPI
  call mpi_init(ierr)

  if (ierr /= MPI_SUCCESS) then
    write (stderr, *) "ERROR: error initializing MPI"
    call exit(1)
  end if
#endif

  node = catalyst_conduit_node_create()
  ! node["data"].set_int32(a);
  call catalyst_conduit_node_set_path_int32(node, "data", a)
  ! node["stage"].set_char8_str("initialize");
  call catalyst_conduit_node_set_path_char8_str(node, "stage", "initialize")
  !node["catalyst_load"]["implementation"].set_char8_str("replay");
  call catalyst_conduit_node_set_path_char8_str(node, "catalyst_load/implementation", "replay")
  !node["catalyst_load"]["search_paths"]["example"].set_char8_str(argv[1]);
  call catalyst_conduit_node_set_path_char8_str(node, "catalyst_load/search_paths/example", trim(arg1))
  err = c_catalyst_initialize(node)
  if (err /= catalyst_status_ok) then
    write (stderr, *) "ERROR: failed to initialize: ", err
    ret = 1
  end if

  call catalyst_conduit_node_set_path_char8_str(node, "stage", "execute")
  a = a + 1
  call catalyst_conduit_node_set_path_int32(node, "data", a)
  do i = 0, 16
    err = c_catalyst_execute(node)
    if (err /= catalyst_status_ok) then
      write (stderr, *) "ERROR: failed to execute: ", i, err
      ret = 1
    end if
  end do

  call catalyst_conduit_node_set_path_char8_str(node, "stage", "finalize")
  a = a + 1
  call catalyst_conduit_node_set_path_int32(node, "data", a)

  err = c_catalyst_finalize(node)
  if (err /= catalyst_status_ok) then
    write (stderr, *) "ERROR: failed to finalize: ", err
    ret = 1
  end if

  call catalyst_conduit_node_set_path_char8_str(node, "stage", "about")
  err = c_catalyst_about(node)
  if (err /= catalyst_status_ok) then
    write (stderr, *) "ERROR: failed to execute about: ", err
    ret = 1
  end if

#ifdef CATALYST_USE_MPI
  call mpi_finalize(ierr)
  if (ierr /= MPI_SUCCESS) then
    write (stderr, *) "ERROR: error finalizing MPI"
    call exit(1)
  end if
#endif

  if (ret /= 0) then
    call exit(1)
  end if

end program
