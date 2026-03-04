/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

#include <catalyst.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Compare paths treating '/' and '\\' as equivalent (for Windows compatibility)
static int paths_equal(const char* a, const char* b)
{
  while (*a && *b)
  {
    char ca = *a;
    char cb = *b;
#ifdef _WIN32
    // Normalize path separators on Windows
    if (ca == '\\')
    {
      ca = '/';
    }
    if (cb == '\\')
    {
      cb = '/';
    }
#endif
    if (ca != cb)
    {
      return 0;
    }
    a++;
    b++;
  }
  return *a == *b; // Both should be '\0'
}

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    fprintf(stderr, "Usage: %s <expected_impl_path>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char* expected_path = argv[1];
  int ret = EXIT_SUCCESS;
  enum catalyst_status err;

  // Initialize with "double" implementation
  conduit_node* params = conduit_node_create();
  conduit_node_set_path_double(params, "data", 1.);
  conduit_node_set_path_char8_str(params, "catalyst_load/implementation", "double");
  err = catalyst_initialize(params);
  conduit_node_destroy(params);
  if (err != catalyst_status_ok)
  {
    fprintf(stderr, "double: failed to initialize: %d\n", err);
    return EXIT_FAILURE;
  }

  // Call catalyst_about and verify implementation path info
  conduit_node* about = conduit_node_create();
  err = catalyst_about(about);
  if (err != catalyst_status_ok)
  {
    fprintf(stderr, "double: failed to call `about`: %d\n", err);
    ret = EXIT_FAILURE;
  }

  // Verify catalyst/version exists
  if (!conduit_node_has_path(about, "catalyst/version"))
  {
    fprintf(stderr, "double: missing `catalyst/version` in about\n");
    ret = EXIT_FAILURE;
  }

  // Verify catalyst/implementation_path exists
  if (!conduit_node_has_path(about, "catalyst/implementation_path"))
  {
    fprintf(stderr, "double: missing `catalyst/implementation_path` in about\n");
    ret = EXIT_FAILURE;
  }

  // Verify catalyst/implementation_source exists
  if (!conduit_node_has_path(about, "catalyst/implementation_source"))
  {
    fprintf(stderr, "double: missing `catalyst/implementation_source` in about\n");
    ret = EXIT_FAILURE;
  }

  // Verify catalyst/implementation_abi_version exists
  if (!conduit_node_has_path(about, "catalyst/implementation_abi_version"))
  {
    fprintf(stderr, "double: missing `catalyst/implementation_abi_version` in about\n");
    ret = EXIT_FAILURE;
  }

  // For loaded impl, implementation_source should be "default" (found in default search path)
  const char* source =
    conduit_node_fetch_path_as_char8_str(about, "catalyst/implementation_source");
  if (source == NULL || strcmp(source, "default") != 0)
  {
    fprintf(stderr, "double: expected implementation_source to be 'default', got '%s'\n",
      source ? source : "(null)");
    ret = EXIT_FAILURE;
  }

  // For loaded impl, implementation_path should match the expected path passed as argv[1]
  // Use paths_equal to handle Windows path separator differences ('/' vs '\\')
  const char* path = conduit_node_fetch_path_as_char8_str(about, "catalyst/implementation_path");
  if (path == NULL)
  {
    fprintf(stderr, "double: implementation_path is NULL\n");
    ret = EXIT_FAILURE;
  }
  else if (!paths_equal(path, expected_path))
  {
    fprintf(
      stderr, "double: expected implementation_path to be '%s', got '%s'\n", expected_path, path);
    ret = EXIT_FAILURE;
  }

  conduit_node_destroy(about);

  // Finalize
  conduit_node* final = conduit_node_create();
  catalyst_finalize(final);
  conduit_node_destroy(final);

  return ret;
}
