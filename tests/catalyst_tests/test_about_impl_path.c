/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

#include <catalyst.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test_stub_impl()
{
  int ret = EXIT_SUCCESS;
  enum catalyst_status err;

  // Initialize with stub implementation (no catalyst_load/implementation specified)
  conduit_node* params = conduit_node_create();
  err = catalyst_initialize(params);
  conduit_node_destroy(params);
  if (err != catalyst_status_ok)
  {
    fprintf(stderr, "stub: failed to initialize: %d\n", err);
    return EXIT_FAILURE;
  }

  // Call catalyst_about and verify implementation path info
  conduit_node* about = conduit_node_create();
  err = catalyst_about(about);
  if (err != catalyst_status_ok)
  {
    fprintf(stderr, "stub: failed to call `about`: %d\n", err);
    ret = EXIT_FAILURE;
  }

  // Verify catalyst/version exists
  if (!conduit_node_has_path(about, "catalyst/version"))
  {
    fprintf(stderr, "stub: missing `catalyst/version` in about\n");
    ret = EXIT_FAILURE;
  }

  // Verify catalyst/implementation_path exists
  if (!conduit_node_has_path(about, "catalyst/implementation_path"))
  {
    fprintf(stderr, "stub: missing `catalyst/implementation_path` in about\n");
    ret = EXIT_FAILURE;
  }

  // Verify catalyst/implementation_source exists
  if (!conduit_node_has_path(about, "catalyst/implementation_source"))
  {
    fprintf(stderr, "stub: missing `catalyst/implementation_source` in about\n");
    ret = EXIT_FAILURE;
  }

  // Verify catalyst/implementation_abi_version exists
  if (!conduit_node_has_path(about, "catalyst/implementation_abi_version"))
  {
    fprintf(stderr, "stub: missing `catalyst/implementation_abi_version` in about\n");
    ret = EXIT_FAILURE;
  }

  // For stub, implementation_source should be "stub"
  const char* source =
    conduit_node_fetch_path_as_char8_str(about, "catalyst/implementation_source");
  if (source == NULL || strcmp(source, "stub") != 0)
  {
    fprintf(stderr, "stub: expected implementation_source to be 'stub', got '%s'\n",
      source ? source : "(null)");
    ret = EXIT_FAILURE;
  }

  // For stub, implementation_path should be "(stub)"
  const char* path = conduit_node_fetch_path_as_char8_str(about, "catalyst/implementation_path");
  if (path == NULL || strcmp(path, "(stub)") != 0)
  {
    fprintf(stderr, "stub: expected implementation_path to be '(stub)', got '%s'\n",
      path ? path : "(null)");
    ret = EXIT_FAILURE;
  }

  conduit_node_destroy(about);

  // Finalize
  conduit_node* final = conduit_node_create();
  catalyst_finalize(final);
  conduit_node_destroy(final);

  return ret;
}

int main(int argc, char* argv[])
{
  (void)argc;
  (void)argv;

  return test_stub_impl();
}
