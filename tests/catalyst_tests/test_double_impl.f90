!/*
! * Distributed under OSI-approved BSD 3-Clause License. See
! * accompanying License.txt
! */

module test_double_impl_fortran
!------------------------------------------------------------------------------

  use, intrinsic :: iso_c_binding, only: C_PTR
  use, intrinsic :: iso_fortran_env, only: stderr => error_unit
  use catalyst_api
  use catalyst_conduit
  implicit none

!------------------------------------------------------------------------------
contains
!------------------------------------------------------------------------------

  subroutine test_double_impl(exit_code)
    type(C_PTR) params, about, exec, final, results
    integer, parameter :: f64 = selected_real_kind(8)
    integer(kind(catalyst_status)) :: code
    integer, intent(out) :: exit_code

    exit_code = 0

    params = catalyst_conduit_node_create()
    !call catalyst_conduit_node_set_path_double(params, "data", 1.0_f64) ! as double is missing from conduit wrappers
    call catalyst_conduit_node_set_path_float64(params, "data", 1.0_f64)
    call catalyst_conduit_node_set_path_char8_str(params, "catalyst_load/implementation", "double")

    code = c_catalyst_initialize(params)
    if (code /= catalyst_status_ok) then
      write (stderr, *) "failed to initialize: ", code
      exit_code = 1
    end if
    call catalyst_conduit_node_destroy(params)

    about = catalyst_conduit_node_create()
    code = c_catalyst_about(about)
    if (code /= catalyst_status_ok) then
      write (stderr, *) "failed to call `about` ", code
      exit_code = 1
    end if

    !if (catalyst_conduit_node_fetch_path_as_double(about, "data") /= 1.0_f64)
    if (catalyst_conduit_node_fetch_path_as_float64(about, "data") /= 1.0_f64) then
      write (stderr, *) "failed to get `data` from `about` ", code
      exit_code = 1
    end if
    call catalyst_conduit_node_destroy(about)

    exec = catalyst_conduit_node_create()
    !conduit_node_set_path_double(exec, "data", 2.0_f64)
    call catalyst_conduit_node_set_path_float64(exec, "data", 2.0_f64)
    code = c_catalyst_execute(exec)
    if (code /= catalyst_status_ok) then
      write (stderr, *) "failed to call `execute`:", code
      exit_code = 1
    end if
    call catalyst_conduit_node_destroy(exec)

    exec = catalyst_conduit_node_create()
    !conduit_node_set_path_double(exec, "data", 2.0_f64)
    call catalyst_conduit_node_set_path_float64(exec, "data", 2.0_f64)
    code = c_catalyst_execute(exec)
    if (code /= catalyst_status_ok) then
      write (stderr, *) "failed to call `execute`:", code
      exit_code = 1
    end if
    call catalyst_conduit_node_destroy(exec)

    results = catalyst_conduit_node_create()
    code = c_catalyst_results(results)
    if (code /= catalyst_status_ok) then
      write (stderr, *) "failed to call `result`:", code
      exit_code = 1
    end if

    if (catalyst_conduit_node_fetch_path_as_float64(results, "data") /= 2.0_f64) then
      write (stderr, *) "failed to get `data` from `results`", code
      exit_code = 1
    end if
    call catalyst_conduit_node_destroy(results)

    final = catalyst_conduit_node_create()
    code = c_catalyst_finalize(final)
    if (code /= catalyst_status_ok) then
      write (stderr, *) "failed to call `finalize`:", code
      exit_code = 1
    end if

    if (catalyst_conduit_node_fetch_path_as_float64(final, "data") /= 2.0_f64) then
      write (stderr, *) "failed to get `data` from `finalize`", code
      exit_code = 1
    end if
    call catalyst_conduit_node_destroy(final)
  end subroutine test_double_impl

end module

program fortran_test
!------------------------------------------------------------------------------
  use test_double_impl_fortran
  implicit none

  integer :: exit_code
  call test_double_impl(exit_code)

  if (exit_code /= 0) then
    call exit(1)
  end if

!------------------------------------------------------------------------------
end program fortran_test
