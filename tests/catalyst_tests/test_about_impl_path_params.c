/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

#include <catalyst.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Test that implementation_source is "params" when loaded via
 * catalyst_load/search_paths/paths parameter.
 *
 * Usage: test_about_impl_path_params <path_to_catalyst_impl_directory>
 *
 * The test sets catalyst_load/search_paths/paths to the provided directory
 * and verifies that catalyst_about reports the correct source.
 */
int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    fprintf(stderr, "Usage: %s <path_to_catalyst_impl_directory>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char* impl_dir = argv[1];
  int ret = EXIT_SUCCESS;
  enum catalyst_status err;

  // Initialize with implementation path specified via params
  conduit_node* params = conduit_node_create();
  conduit_node_set_path_char8_str(params, "catalyst_load/implementation", "double");
  conduit_node_set_path_char8_str(params, "catalyst_load/search_paths/paths", impl_dir);
  err = catalyst_initialize(params);
  conduit_node_destroy(params);
  if (err != catalyst_status_ok)
  {
    fprintf(stderr, "params: failed to initialize: %d\n", err);
    return EXIT_FAILURE;
  }

  // Call catalyst_about and verify implementation path info
  conduit_node* about = conduit_node_create();
  err = catalyst_about(about);
  if (err != catalyst_status_ok)
  {
    fprintf(stderr, "params: failed to call `about`: %d\n", err);
    ret = EXIT_FAILURE;
  }

  // Verify catalyst/version exists
  if (!conduit_node_has_path(about, "catalyst/version"))
  {
    fprintf(stderr, "params: missing `catalyst/version` in about\n");
    ret = EXIT_FAILURE;
  }

  // Verify catalyst/implementation_path exists and contains the impl directory
  if (!conduit_node_has_path(about, "catalyst/implementation_path"))
  {
    fprintf(stderr, "params: missing `catalyst/implementation_path` in about\n");
    ret = EXIT_FAILURE;
  }
  else
  {
    const char* path = conduit_node_fetch_path_as_char8_str(about, "catalyst/implementation_path");
    if (path == NULL || strstr(path, "double") == NULL)
    {
      fprintf(stderr, "params: expected implementation_path to contain 'double', got '%s'\n",
        path ? path : "(null)");
      ret = EXIT_FAILURE;
    }
  }

  // Verify catalyst/implementation_source is "params"
  if (!conduit_node_has_path(about, "catalyst/implementation_source"))
  {
    fprintf(stderr, "params: missing `catalyst/implementation_source` in about\n");
    ret = EXIT_FAILURE;
  }
  else
  {
    const char* source =
      conduit_node_fetch_path_as_char8_str(about, "catalyst/implementation_source");
    if (source == NULL || strcmp(source, "params") != 0)
    {
      fprintf(stderr, "params: expected implementation_source to be 'params', got '%s'\n",
        source ? source : "(null)");
      ret = EXIT_FAILURE;
    }
  }

  // Verify catalyst/implementation_abi_version exists
  if (!conduit_node_has_path(about, "catalyst/implementation_abi_version"))
  {
    fprintf(stderr, "params: missing `catalyst/implementation_abi_version` in about\n");
    ret = EXIT_FAILURE;
  }

  conduit_node_destroy(about);

  // Finalize
  conduit_node* final = conduit_node_create();
  catalyst_finalize(final);
  conduit_node_destroy(final);

  return ret;
}
