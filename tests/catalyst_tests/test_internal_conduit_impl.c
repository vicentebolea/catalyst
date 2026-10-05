/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

#include <catalyst.h>

#include <stdio.h>
#include <stdlib.h>

int test_internal_conduit_impl()
{
  int ret = EXIT_SUCCESS;
  enum catalyst_status err;

  conduit_node* params = conduit_node_create();
  conduit_node_set_path_char8_str(params, "catalyst_load/implementation", "internal_conduit");
  err = catalyst_initialize(params);
  conduit_node_destroy(params);

#if !CATALYST_WITH_EXTERNAL_CONDUIT
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

  // main must not compile to the same code as test_internal_conduit_impl: with
  // -O2 or higher, Apple's linker folds identical functions and then loses the
  // entry point (LC_MAIN entryoff 0), so the test crashes before main runs.
  const int ret = test_internal_conduit_impl();
  if (ret != EXIT_SUCCESS)
  {
    fprintf(stderr, "test_internal_conduit_impl failed\n");
  }
  return ret;
}
