/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

/*
 * An implementation making MPI collectives on the communicator it is given, as
 * implementations such as ParaView Catalyst do, and checking their results. In
 * async mode, these run on the worker thread while the simulation thread makes
 * its own collectives, and Catalyst its skip decisions.
 */

#include <catalyst.h>
#include <catalyst_conduit.hpp>
#include <catalyst_stub.h>

#include <mpi.h>

#include <cstdio>

#include "catalyst_impl_async_mpi.h"

namespace
{
MPI_Comm comm = MPI_COMM_NULL;
long reductions = 0;
long wrong_results = 0;

// Collectives made by each execute.
constexpr int kReductionsPerExecute = 200;
// The driver reduces rank + 1. Reducing a multiple here makes a reduction of
// this thread matched with one of the driver's on another rank give a wrong sum.
constexpr int kValueScale = 1000;
}

//-----------------------------------------------------------------------------
enum catalyst_status catalyst_initialize_async_mpi(const conduit_node* params)
{
  auto node = conduit_cpp::cpp_node(const_cast<conduit_node*>(params));
  comm = MPI_COMM_WORLD;
  if (node.has_path("catalyst/mpi_comm"))
  {
    comm = MPI_Comm_f2c(static_cast<MPI_Fint>(node["catalyst/mpi_comm"].to_int64()));
  }
  reductions = 0;
  wrong_results = 0;
  return catalyst_status_ok;
}

//-----------------------------------------------------------------------------
enum catalyst_status catalyst_execute_async_mpi(const conduit_node* /*params*/)
{
  int rank = 0;
  int size = 1;
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);
  const int expected = kValueScale * size * (size + 1) / 2;
  for (int i = 0; i < kReductionsPerExecute; ++i)
  {
    const int local = kValueScale * (rank + 1);
    int sum = 0;
    MPI_Allreduce(&local, &sum, 1, MPI_INT, MPI_SUM, comm);
    ++reductions;
    if (sum != expected)
    {
      ++wrong_results;
    }
  }
  return catalyst_status_ok;
}

//-----------------------------------------------------------------------------
enum catalyst_status catalyst_finalize_async_mpi(const conduit_node* /*params*/)
{
  int rank = 0;
  MPI_Comm_rank(comm, &rank);
  if (wrong_results != 0)
  {
    fprintf(stderr, "async_mpi: rank %d: %ld of %ld reductions returned a wrong result\n", rank,
      wrong_results, reductions);
  }
  comm = MPI_COMM_NULL;
  return wrong_results == 0 ? catalyst_status_ok : catalyst_status_error_incomplete;
}

//-----------------------------------------------------------------------------
enum catalyst_status catalyst_about_async_mpi(conduit_node* params)
{
  return catalyst_stub_about(params);
}

//-----------------------------------------------------------------------------
enum catalyst_status catalyst_results_async_mpi(conduit_node* params)
{
  return catalyst_stub_results(params);
}
