/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

/**
 * `libcatalyst_mpi`: the only part of Catalyst compiled against a real `mpi.h`.
 * Built per-MPI and loaded at runtime; see `catalyst_mpi_ops.h`.
 */

#include <mpi.h>

#include "catalyst_mpi_ops.h"

#ifdef _WIN32
#define CATALYST_MPI_EXPORT __declspec(dllexport)
#else
#define CATALYST_MPI_EXPORT __attribute__((visibility("default")))
#endif

static MPI_Comm resolve_comm(int64_t comm_f)
{
  if (comm_f == CATALYST_MPI_COMM_WORLD)
  {
    return MPI_COMM_WORLD;
  }
  if (comm_f == CATALYST_MPI_NO_COMM)
  {
    return MPI_COMM_NULL;
  }
  return MPI_Comm_f2c((MPI_Fint)comm_f);
}

static int shim_is_usable(void)
{
  int initialized = 0;
  int finalized = 0;
  if (MPI_Initialized(&initialized) != MPI_SUCCESS)
  {
    return 0;
  }
  if (initialized && MPI_Finalized(&finalized) != MPI_SUCCESS)
  {
    return 0;
  }
  return initialized && !finalized;
}

static int shim_comm_rank(int64_t comm_f, int* rank)
{
  MPI_Comm comm = resolve_comm(comm_f);
  if (comm == MPI_COMM_NULL)
  {
    return CATALYST_MPI_FAIL;
  }
  const int err = MPI_Comm_rank(comm, rank);
  return err == MPI_SUCCESS ? CATALYST_MPI_OK : CATALYST_MPI_FAIL;
}

static int shim_comm_size(int64_t comm_f, int* size)
{
  MPI_Comm comm = resolve_comm(comm_f);
  if (comm == MPI_COMM_NULL)
  {
    return CATALYST_MPI_FAIL;
  }
  const int err = MPI_Comm_size(comm, size);
  return err == MPI_SUCCESS ? CATALYST_MPI_OK : CATALYST_MPI_FAIL;
}

static int shim_allreduce_min_int(int64_t comm_f, int local, int* out)
{
  MPI_Comm comm = resolve_comm(comm_f);
  if (comm == MPI_COMM_NULL)
  {
    return CATALYST_MPI_FAIL;
  }
  const int err = MPI_Allreduce(&local, out, 1, MPI_INT, MPI_MIN, comm);
  return err == MPI_SUCCESS ? CATALYST_MPI_OK : CATALYST_MPI_FAIL;
}

static int shim_bcast_int(int64_t comm_f, int* value, int root)
{
  MPI_Comm comm = resolve_comm(comm_f);
  if (comm == MPI_COMM_NULL)
  {
    return CATALYST_MPI_FAIL;
  }
  const int err = MPI_Bcast(value, 1, MPI_INT, root, comm);
  return err == MPI_SUCCESS ? CATALYST_MPI_OK : CATALYST_MPI_FAIL;
}

static int shim_comm_dup(int64_t comm_f, int64_t* out_f)
{
  MPI_Comm comm = resolve_comm(comm_f);
  if (comm == MPI_COMM_NULL)
  {
    return CATALYST_MPI_FAIL;
  }
  MPI_Comm dup = MPI_COMM_NULL;
  if (MPI_Comm_dup(comm, &dup) != MPI_SUCCESS)
  {
    return CATALYST_MPI_FAIL;
  }
  *out_f = (int64_t)MPI_Comm_c2f(dup);
  return CATALYST_MPI_OK;
}

static int shim_comm_free(int64_t comm_f)
{
  MPI_Comm comm = resolve_comm(comm_f);
  if (comm == MPI_COMM_NULL || comm == MPI_COMM_WORLD)
  {
    return CATALYST_MPI_FAIL;
  }
  const int err = MPI_Comm_free(&comm);
  return err == MPI_SUCCESS ? CATALYST_MPI_OK : CATALYST_MPI_FAIL;
}

static const struct catalyst_mpi_ops catalyst_mpi_ops_table = {
  CATALYST_MPI_OPS_VERSION,
  shim_is_usable,
  shim_comm_rank,
  shim_comm_size,
  shim_allreduce_min_int,
  shim_bcast_int,
  shim_comm_dup,
  shim_comm_free,
};

CATALYST_MPI_EXPORT const struct catalyst_mpi_ops* catalyst_mpi_get_ops(void)
{
  return &catalyst_mpi_ops_table;
}
