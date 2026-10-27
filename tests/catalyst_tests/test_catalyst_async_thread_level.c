/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

/*
 * Async execution needs MPI_THREAD_MULTIPLE. Initialize MPI at the level named
 * on the command line ("single" or "multiple"), request async, and check that
 * Catalyst enabled it only under MPI_THREAD_MULTIPLE.
 */

#include <catalyst.h>
#include <mpi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
  const int want_multiple = argc > 1 && strcmp(argv[1], "multiple") == 0;

  int provided = MPI_THREAD_SINGLE;
  MPI_Init_thread(&argc, &argv, want_multiple ? MPI_THREAD_MULTIPLE : MPI_THREAD_SINGLE, &provided);
  if (want_multiple && provided != MPI_THREAD_MULTIPLE)
  {
    printf("This MPI does not provide MPI_THREAD_MULTIPLE; skipping.\n");
    MPI_Finalize();
    return 125;
  }

  conduit_node* params = conduit_node_create();
  conduit_node_set_path_int64(params, "catalyst/async/enabled", 1);
  conduit_node_set_path_int64(params, "catalyst/async/verbose", 0);
  conduit_node_set_path_char8_str(params, "catalyst/async/affinity/mode", "none");
  enum catalyst_status err = catalyst_initialize(params);
  conduit_node_destroy(params);
  if (err != catalyst_status_ok)
  {
    fprintf(stderr, "failed to initialize: %d\n", err);
    MPI_Finalize();
    return EXIT_FAILURE;
  }

  conduit_node* about = conduit_node_create();
  catalyst_about(about);
  const int enabled = conduit_node_has_path(about, "catalyst/async/enabled") &&
    conduit_node_fetch_path_as_int(about, "catalyst/async/enabled") != 0;
  conduit_node_destroy(about);

  conduit_node* final_params = conduit_node_create();
  catalyst_finalize(final_params);
  conduit_node_destroy(final_params);
  MPI_Finalize();

  if (enabled != want_multiple)
  {
    fprintf(stderr, "async enabled = %d with MPI initialized at %s; expected %d\n", enabled,
      want_multiple ? "MPI_THREAD_MULTIPLE" : "MPI_THREAD_SINGLE", want_multiple);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
