/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

/*
 * Tests that, in async mode, the collectives of the implementation on the
 * worker thread do not interfere with those of the simulation, nor with the
 * skip decisions of Catalyst, on the simulation thread.
 *
 * The `async_mpi` implementation checks the results of its collectives and
 * fails `catalyst_finalize` if any is wrong. This driver makes collectives of
 * its own between executes, as a simulation does, and checks their results
 * too. It runs three times: on MPI_COMM_WORLD, on MPI_COMM_WORLD again after
 * finalizing, and on a communicator split from it and passed as
 * `catalyst/mpi_comm`. Run with several ranks.
 *
 * Usage: test_catalyst_async_mpi <path_to_catalyst_impl_directory>
 *
 * Returns 125 (skipped) when MPI does not provide MPI_THREAD_MULTIPLE.
 */

#include <catalyst.h>
#include <catalyst_conduit.hpp>

#include <mpi.h>

#include <cstdio>
#include <cstdlib>

namespace
{
constexpr int kSteps = 200;
// Both threads reduce continuously so that their collectives overlap.
constexpr int kReductionsPerStep = 200;

// Runs Catalyst in async mode with the simulation on `comm`, which is passed
// as `catalyst/mpi_comm` unless it is MPI_COMM_WORLD. Returns 1 on failure.
int RunCase(const char* name, MPI_Comm comm, const char* implDir)
{
  int rank = 0;
  int size = 1;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);
  int failed = 0;

  conduit_cpp::Node params;
  params["catalyst_load/implementation"] = "async_mpi";
  params["catalyst_load/search_paths/paths"] = implDir;
  params["catalyst/async/enabled"] = 1;
  params["catalyst/async/queue_depth"] = 4;
  if (comm != MPI_COMM_WORLD)
  {
    params["catalyst/mpi_comm"].set_int64(MPI_Comm_c2f(comm));
  }
  enum catalyst_status err = catalyst_initialize(conduit_cpp::c_node(&params));
  if (err != catalyst_status_ok)
  {
    fprintf(stderr, "%s: rank %d: failed to initialize: %d\n", name, rank, err);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  conduit_cpp::Node about;
  catalyst_about(conduit_cpp::c_node(&about));
  if (!about.has_path("catalyst/async/enabled") || about["catalyst/async/enabled"].to_int() != 1)
  {
    fprintf(stderr, "%s: rank %d: async execution is not enabled\n", name, rank);
    failed = 1;
  }

  const int expected = size * (size + 1) / 2;
  long wrong_results = 0;
  for (int step = 0; step < kSteps; ++step)
  {
    for (int i = 0; i < kReductionsPerStep; ++i)
    {
      const int local = rank + 1;
      int sum = 0;
      MPI_Allreduce(&local, &sum, 1, MPI_INT, MPI_SUM, comm);
      if (sum != expected)
      {
        ++wrong_results;
      }
    }

    conduit_cpp::Node exec_params;
    exec_params["catalyst/state/timestep"] = step;
    exec_params["catalyst/state/time"] = static_cast<double>(step);
    err = catalyst_execute(conduit_cpp::c_node(&exec_params));
    if (err != catalyst_status_ok)
    {
      fprintf(stderr, "%s: rank %d: failed to execute step %d: %d\n", name, rank, step, err);
      failed = 1;
    }
  }

  const conduit_cpp::Node fin_params;
  err = catalyst_finalize(conduit_cpp::c_node(&fin_params));
  if (err != catalyst_status_ok)
  {
    fprintf(stderr,
      "%s: rank %d: catalyst_finalize failed (%d): the implementation's collectives "
      "returned wrong results\n",
      name, rank, err);
    failed = 1;
  }
  if (wrong_results != 0)
  {
    fprintf(stderr, "%s: rank %d: %ld of %d simulation reductions returned a wrong result\n", name,
      rank, wrong_results, kSteps * kReductionsPerStep);
    failed = 1;
  }
  return failed;
}
}

// NOLINTNEXTLINE(bugprone-exception-escape) - test code using conduit_cpp
int main(int argc, char* argv[])
{
  int provided = MPI_THREAD_SINGLE;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (provided < MPI_THREAD_MULTIPLE)
  {
    if (rank == 0)
    {
      printf("MPI does not provide MPI_THREAD_MULTIPLE; skipping.\n");
    }
    MPI_Finalize();
    return 125;
  }
  if (argc < 2)
  {
    fprintf(stderr, "Usage: %s <path_to_catalyst_impl_directory>\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  int failed = 0;
  failed |= RunCase("world", MPI_COMM_WORLD, argv[1]);
  failed |= RunCase("world after finalize", MPI_COMM_WORLD, argv[1]);

  MPI_Comm half = MPI_COMM_NULL;
  MPI_Comm_split(MPI_COMM_WORLD, rank % 2, rank, &half);
  failed |= RunCase("split communicator", half, argv[1]);
  MPI_Comm_free(&half);

  int any_failed = 0;
  MPI_Allreduce(&failed, &any_failed, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
  MPI_Finalize();
  return any_failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
