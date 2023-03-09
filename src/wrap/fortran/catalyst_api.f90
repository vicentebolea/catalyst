! Distributed under OSI-approved BSD 3-Clause License. See
! accompanying License.txt

module catalyst_api

  use iso_c_binding
  use conduit

  implicit none

  public c_catalyst_initialize, c_catalyst_execute, c_catalyst_finalize, c_catalyst_about, c_catalyst_results

  enum, bind(c)

    ! Use this with `integer(kind(catalyst_status))` to declare variables of this type.
    enumerator :: catalyst_status = 0

    enumerator :: catalyst_status_ok = 0
    enumerator :: catalyst_status_error_no_implementation = 1
    enumerator :: catalyst_status_error_already_loaded = 2
    enumerator :: catalyst_status_error_not_found = 3
    enumerator :: catalyst_status_error_not_catalyst = 4
    enumerator :: catalyst_status_error_incomplete = 5
    enumerator :: catalyst_status_error_unsupported_version = 6
    enumerator :: catalyst_status_error_conduit_mismatch = 7

  end enum

  interface

    function c_catalyst_initialize(cnode) result(catalyst_code) bind(C, name='catalyst_initialize')
      use, intrinsic :: iso_c_binding, only: c_int, c_ptr

      implicit none

      type(c_ptr), value, intent(in) :: cnode
      integer(c_int) :: catalyst_code
    end function c_catalyst_initialize

    function c_catalyst_execute(node) result(catalyst_code) bind(C, name='catalyst_execute')
      use, intrinsic :: iso_c_binding, only: c_int, c_ptr

      implicit none

      type(c_ptr), value, intent(in) :: node
      integer(c_int) :: catalyst_code
    end function c_catalyst_execute

    function c_catalyst_finalize(node) result(catalyst_code) bind(C, name='catalyst_finalize')
      use, intrinsic :: iso_c_binding, only: c_int, c_ptr

      implicit none

      type(c_ptr), value, intent(in) :: node
      integer(c_int) :: catalyst_code
    end function c_catalyst_finalize

    function c_catalyst_about(node) result(catalyst_code) bind(C, name='catalyst_about')
      use, intrinsic :: iso_c_binding, only: c_int, c_ptr

      implicit none

      type(c_ptr), value :: node
      integer(c_int) :: catalyst_code
    end function c_catalyst_about

    function c_catalyst_results(node) result(catalyst_status) bind(C, name='catalyst_results')
      use, intrinsic :: iso_c_binding, only: c_int, c_ptr

      implicit none

      type(c_ptr), value :: node
      integer(c_int) :: catalyst_status
    end function c_catalyst_results
  end interface

end module catalyst_api
