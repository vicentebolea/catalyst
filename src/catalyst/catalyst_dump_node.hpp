/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

#ifndef catalyst_dump_node_h
#define catalyst_dump_node_h

#include "catalyst_mpi_ops.h"

#include <conduit_utils.hpp>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined(_WIN32) && !defined(__CYGWIN__)
#define PATH_SEP '\\'
#else
#define PATH_SEP '/'
#endif

// Failures are reported rather than ignored: silently taking rank 0 everywhere
// would have every rank write the same file.
inline void get_mpi_info(int& rank, int& num_ranks)
{
  rank = 0;
  num_ranks = 1;

  const struct catalyst_mpi_ops* ops = catalyst_mpi_get_loaded_ops();
  if (!ops)
  {
#ifdef CATALYST_USE_MPI
    // catalyst_initialize refuses to proceed without the module, so this means
    // it was unloaded underneath us.
    fprintf(stderr, "catalyst: MPI operations unavailable; assuming serial output\n");
#endif
    return;
  }

  if (!ops->is_usable())
  {
    return;
  }

  if (ops->comm_rank(CATALYST_MPI_COMM_WORLD, &rank) != CATALYST_MPI_OK ||
    ops->comm_size(CATALYST_MPI_COMM_WORLD, &num_ranks) != CATALYST_MPI_OK)
  {
    fprintf(stderr, "catalyst: could not determine MPI rank; assuming serial output\n");
    rank = 0;
    num_ranks = 1;
  }
}

// Constructs the absolute path of the file to write out.
// Similar to python's os.path.join.
inline char* construct_full_path(
  const char* out_dir, const char* stage, unsigned long invocations, int use_invocations)
{
  int num_ranks = 1;
  int rank = 0;
  get_mpi_info(rank, num_ranks);

  const size_t out_dir_len = strlen(out_dir);
  if (!out_dir_len)
  {
    fprintf(stderr, "Empty data_dump_directory detected.\n");
    exit(1);
  }

  // Add a separator to the end of the directory if needed.
  std::array<char, 2> sep_or_no_sep = { 0, 0 };
  if (out_dir[out_dir_len - 1] != PATH_SEP)
  {
    sep_or_no_sep[0] = PATH_SEP;
  }

  size_t full_path_len = 0;
  char* full_path = nullptr;

  // If we want to include the invocation number in the filename,
  // use a different format for the filename
  if (use_invocations)
  {
    // Determine the length of the path
    full_path_len = snprintf(nullptr, 0, "%s%s%s_invc%lu_params.conduit_bin.%d.%d", out_dir,
      sep_or_no_sep.data(), stage, invocations, num_ranks, rank);

    // Format the string with the correct information
    full_path = (char*)calloc(full_path_len + 1, sizeof(char));
    snprintf(full_path, full_path_len + 1, "%s%s%s_invc%lu_params.conduit_bin.%d.%d", out_dir,
      sep_or_no_sep.data(), stage, invocations, num_ranks, rank);
  }
  else
  {
    // Determine the length of the path
    full_path_len = snprintf(nullptr, 0, "%s%s%s_params.conduit_bin.%d.%d", out_dir,
      sep_or_no_sep.data(), stage, num_ranks, rank);

    // Format the string with the correct information
    full_path = (char*)calloc(full_path_len + 1, sizeof(char));
    snprintf(full_path, full_path_len + 1, "%s%s%s_params.conduit_bin.%d.%d", out_dir,
      sep_or_no_sep.data(), stage, num_ranks, rank);
  }

  if (!(full_path_len && full_path))
  {
    fprintf(stderr, "Invalid dump_node filename convention.\n");
    exit(1);
  }

  return full_path;
}

inline bool ensure_directory(const char* out_dir)
{
  int rank = 0;
  int num_ranks = 1;
  get_mpi_info(rank, num_ranks);

  int directory_exists = 0;
  if (rank == 0)
  {
    directory_exists =
      (conduit::utils::is_directory(out_dir) || conduit::utils::create_directory(out_dir)) ? 1 : 0;
  }
  if (num_ranks > 1)
  {
    // Every rank must reach the same conclusion, or some will write into a
    // directory the others believe does not exist.
    const struct catalyst_mpi_ops* ops = catalyst_mpi_get_loaded_ops();
    if (!ops || ops->bcast_int(CATALYST_MPI_COMM_WORLD, &directory_exists, 0) != CATALYST_MPI_OK)
    {
      fprintf(stderr,
        "catalyst: could not agree across ranks on whether `%s` exists; not writing\n", out_dir);
      return false;
    }
  }
  return (directory_exists == 1);
}

// Check if a data dump directory has been set. If it has,
// write out the params argument to disk to load later.
inline void dump_node(
  const conduit_node* params, const char* stage, unsigned long invocations, int use_invocations)
{
  const char* out_dir = getenv("CATALYST_DATA_DUMP_DIRECTORY");
  if (out_dir && ensure_directory(out_dir))
  {
    char* full_path = construct_full_path(out_dir, stage, invocations, use_invocations);
    conduit_node_save((conduit_node*)(params), full_path, "conduit_bin");
    free(full_path);
  }
}

#endif
