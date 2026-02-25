/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

#include <catalyst.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Test that implementation_source is "environment" when loaded via
 * CATALYST_IMPLEMENTATION_PATHS environment variable.
 *
 * Usage: test_about_impl_path_env <path_to_catalyst_impl_directory>
 *
 * The test sets CATALYST_IMPLEMENTATION_PATHS to the provided directory
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

  // Set environment variable to load implementation from specified directory
  // The directory should contain libcatalyst-double.so (or similar)
#ifdef _WIN32
  size_t env_len = strlen("CATALYST_IMPLEMENTATION_PATHS=") + strlen(impl_dir) + 1;
  char* env_buf = (char*)malloc(env_len);
  snprintf(env_buf, env_len, "CATALYST_IMPLEMENTATION_PATHS=%s", impl_dir);
  _putenv(env_buf);
  _putenv("CATALYST_IMPLEMENTATION_NAME=double");
  // Note: env_buf is intentionally not freed as _putenv may store the pointer
#else
  setenv("CATALYST_IMPLEMENTATION_PATHS", impl_dir, 1);
  setenv("CATALYST_IMPLEMENTATION_NAME", "double", 1);
#endif

  // Initialize - should pick up the implementation from environment
  conduit_node* params = conduit_node_create();
  err = catalyst_initialize(params);
  conduit_node_destroy(params);
  if (err != catalyst_status_ok)
  {
    fprintf(stderr, "env: failed to initialize: %d\n", err);
    return EXIT_FAILURE;
  }

  // Call catalyst_about and verify implementation info
  conduit_node* about = conduit_node_create();
  err = catalyst_about(about);
  if (err != catalyst_status_ok)
  {
    fprintf(stderr, "env: failed to call `about`: %d\n", err);
    ret = EXIT_FAILURE;
  }

  // Verify catalyst/implementation_source is "environment"
  if (!conduit_node_has_path(about, "catalyst/implementation_source"))
  {
    fprintf(stderr, "env: missing `catalyst/implementation_source` in about\n");
    ret = EXIT_FAILURE;
  }
  else
  {
    const char* source =
      conduit_node_fetch_path_as_char8_str(about, "catalyst/implementation_source");
    if (source == NULL || strcmp(source, "environment") != 0)
    {
      fprintf(stderr, "env: expected implementation_source to be 'environment', got '%s'\n",
        source ? source : "(null)");
      ret = EXIT_FAILURE;
    }
  }

  // Verify catalyst/implementation_path contains the directory we specified
  if (!conduit_node_has_path(about, "catalyst/implementation_path"))
  {
    fprintf(stderr, "env: missing `catalyst/implementation_path` in about\n");
    ret = EXIT_FAILURE;
  }
  else
  {
    const char* path = conduit_node_fetch_path_as_char8_str(about, "catalyst/implementation_path");
    if (path == NULL || strstr(path, "double") == NULL)
    {
      fprintf(stderr, "env: expected implementation_path to contain 'double', got '%s'\n",
        path ? path : "(null)");
      ret = EXIT_FAILURE;
    }
  }

  conduit_node_destroy(about);

  // Finalize
  conduit_node* final = conduit_node_create();
  catalyst_finalize(final);
  conduit_node_destroy(final);

  return ret;
}
