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
 * its own on `MPI_COMM_WORLD` between executes, as a simulation does, and
 * checks their results too. Run with several ranks.
 *
 * Usage: test_catalyst_async_mpi <path_to_catalyst_impl_directory>
 *
 * Returns 125 (skipped) when MPI does not provide MPI_THREAD_MULTIPLE.
 */

#include <catalyst.h>
#include <catalyst_conduit.hpp>

#include <mpi.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>

namespace
{
constexpr int kSteps = 200;
constexpr int kReductionsPerStep = 20;
constexpr auto kWorkPerStep = std::chrono::milliseconds(2);

// Stands in for the simulation's computation, giving the worker time to run.
void work()
{
  const auto deadline = std::chrono::steady_clock::now() + kWorkPerStep;
  while (std::chrono::steady_clock::now() < deadline)
  {
  }
}
}

int main(int argc, char* argv[])
{
  int provided = MPI_THREAD_SINGLE;
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  int rank = 0;
  int size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

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

  conduit_cpp::Node params;
  params["catalyst_load/implementation"] = "async_mpi";
  params["catalyst_load/search_paths/paths"] = argv[1];
  params["catalyst/async/enabled"] = 1;
  params["catalyst/async/queue_depth"] = 4;
  enum catalyst_status err = catalyst_initialize(conduit_cpp::c_node(&params));
  if (err != catalyst_status_ok)
  {
    fprintf(stderr, "rank %d: failed to initialize: %d\n", rank, err);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  conduit_cpp::Node about;
  catalyst_about(conduit_cpp::c_node(&about));
  if (!about.has_path("catalyst/async/enabled") || about["catalyst/async/enabled"].to_int() != 1)
  {
    fprintf(stderr, "rank %d: async execution is not enabled\n", rank);
    failed = 1;
  }

  const int expected = size * (size + 1) / 2;
  long wrong_results = 0;
  for (int step = 0; step < kSteps; ++step)
  {
    work();
    for (int i = 0; i < kReductionsPerStep; ++i)
    {
      const int local = rank + 1;
      int sum = 0;
      MPI_Allreduce(&local, &sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
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
      fprintf(stderr, "rank %d: failed to execute step %d: %d\n", rank, step, err);
      failed = 1;
    }
  }

  conduit_cpp::Node fin_params;
  err = catalyst_finalize(conduit_cpp::c_node(&fin_params));
  if (err != catalyst_status_ok)
  {
    fprintf(stderr,
      "rank %d: catalyst_finalize failed (%d): the implementation's collectives "
      "returned wrong results\n",
      rank, err);
    failed = 1;
  }
  if (wrong_results != 0)
  {
    fprintf(stderr, "rank %d: %ld of %d simulation reductions returned a wrong result\n", rank,
      wrong_results, kSteps * kReductionsPerStep);
    failed = 1;
  }

  int any_failed = 0;
  MPI_Allreduce(&failed, &any_failed, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
  MPI_Finalize();
  return any_failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
