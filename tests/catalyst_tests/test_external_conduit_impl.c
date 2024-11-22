/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

#include <catalyst.h>

#include <stdio.h>
#include <stdlib.h>

// Xcode 16.0 and 16.1 have a bug in the linker that optimizes out `main`.
// Experimentation shows that marking `test_external_conduit_impl` as noinline
// prevents this optimization. Reported in person to Apple developers (who
// managed to reproduce the problem as well).
#if defined(__clang__) && defined(__apple_build_version__)
#if (__clang_major__ == 16 && __clang_minor__ == 0) ||                                             \
  (__clang_major__ == 16 && __clang_minor__ == 1)
#define catalyst_xcode16_fix __attribute__((noinline))
#endif
#endif
#ifndef catalyst_xcode16_fix
#define catalyst_xcode16_fix
#endif

int catalyst_xcode16_fix test_external_conduit_impl()
{
  int ret = EXIT_SUCCESS;
  enum catalyst_status err;

  conduit_node* params = conduit_node_create();
  conduit_node_set_path_char8_str(params, "catalyst_load/implementation", "external_conduit");
  err = catalyst_initialize(params);
  conduit_node_destroy(params);

#if CATALYST_WITH_EXTERNAL_CONDUIT
  if (err != catalyst_status_ok)
  {
    fprintf(stderr, "failed to initialize: %d\n", err);
    ret = EXIT_FAILURE;
    return ret;
  }

  conduit_node* final = conduit_node_create();
  err = catalyst_finalize(final);
  if (err != catalyst_status_ok)
  {
    fprintf(stderr, "failed to call `finalize`: %d\n", err);
    ret = EXIT_FAILURE;
  }
  conduit_node_destroy(final);
#else
  if (err != catalyst_status_error_conduit_mismatch)
  {
    fprintf(stderr, "failed to fail to initialize: %d\n", err);
    ret = EXIT_FAILURE;
    return ret;
  }
#endif

  return ret;
}

int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;

  int ret = EXIT_SUCCESS;

  int test_ret = test_external_conduit_impl();
  if (test_ret != EXIT_SUCCESS)
  {
    ret = test_ret;
  }

  return ret;
}
