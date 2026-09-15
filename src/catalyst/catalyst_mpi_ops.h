/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

#ifndef catalyst_mpi_ops_h
#define catalyst_mpi_ops_h

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * Boundary between `libcatalyst` and the MPI it runs against.
   *
   * `libcatalyst` is built without `mpi.h` and never names an MPI type or
   * constant. The operations below are provided by `libcatalyst_mpi`, a small
   * module compiled against a real `mpi.h` and loaded at runtime from the same
   * directory as the Catalyst implementations. Because that module sees the
   * real header, `MPI_COMM_WORLD`, `MPI_INT` and `MPI_MIN` are correct by
   * construction rather than reconstructed from a table of per-implementation
   * values. That is why the operations live in a module rather than being
   * resolved symbol by symbol: those constants are compile-time values with no
   * stable representation across MPI implementations.
   *
   * Only Fortran communicator handles and plain ints cross this boundary.
   */

/** Passed as `comm_f` to operate on the equivalent of `MPI_COMM_WORLD`. */
#define CATALYST_MPI_COMM_WORLD ((int64_t)-1)

  enum catalyst_mpi_result
  {
    /** Returned by every operation on success. */
    CATALYST_MPI_OK = 0,
    /** Returned by every operation on failure. */
    CATALYST_MPI_FAIL = 1
  };

  struct catalyst_mpi_ops
  {
    /** ABI guard; must equal `CATALYST_MPI_OPS_VERSION`. */
    int version;

    /** Non-zero when MPI is initialized and not yet finalized. */
    // NOLINTNEXTLINE(modernize-redundant-void-arg): C header, (void) is required
    int (*is_usable)(void);

    int (*comm_rank)(int64_t comm_f, int* rank);
    int (*comm_size)(int64_t comm_f, int* size);

    /** All-reduce a single int with `MPI_MIN`. */
    int (*allreduce_min_int)(int64_t comm_f, int local, int* out);

    /** Broadcast a single int from `root`. */
    int (*bcast_int)(int64_t comm_f, int* value, int root);
  };

  enum
  {
    CATALYST_MPI_OPS_VERSION = 1
  };

  /**
   * Entry point exported by `libcatalyst_mpi`. Returns a table owned by the
   * module, or NULL if it cannot provide one.
   */
  // NOLINTNEXTLINE(modernize-use-using,modernize-redundant-void-arg): C header
  typedef const struct catalyst_mpi_ops* (*catalyst_mpi_ops_fn)(void);

#define CATALYST_MPI_OPS_SYMBOL "catalyst_mpi_get_ops"

  /**
   * Loads the MPI module. Returns 0 on success. On failure it has already
   * explained the problem on stderr.
   */
  int catalyst_mpi_load(void);

  /**
   * Returns the loaded table, or NULL when Catalyst was built without MPI or
   * before a successful `catalyst_initialize`.
   */
  const struct catalyst_mpi_ops* catalyst_mpi_get_loaded_ops(void);

#ifdef __cplusplus
}
#endif

#endif
