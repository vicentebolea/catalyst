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
   * Boundary between `libcatalyst` and the MPI it runs against, provided by
   * `libcatalyst_mpi` and loaded at runtime. See docs/runtime_linkage.rst.
   */

/** Sentinels for `comm_f`; any other value is an `MPI_Comm_c2f` handle. */
#define CATALYST_MPI_COMM_WORLD ((int64_t)-1)
#define CATALYST_MPI_NO_COMM ((int64_t)-2)

  // NOLINTNEXTLINE(performance-enum-size): C ABI boundary; the operations return int
  enum catalyst_mpi_result
  {
    CATALYST_MPI_OK = 0,
    CATALYST_MPI_FAIL = 1
  };

  struct catalyst_mpi_ops
  {
    /** Must equal `CATALYST_MPI_OPS_VERSION`. */
    int version;

    /** Non-zero when MPI is initialized and not yet finalized. */
    // NOLINTNEXTLINE(modernize-redundant-void-arg): C header, (void) is required
    int (*is_usable)(void);

    int (*comm_rank)(int64_t comm_f, int* rank);
    int (*comm_size)(int64_t comm_f, int* size);

    int (*allreduce_min_int)(int64_t comm_f, int local, int* out);
    int (*bcast_int)(int64_t comm_f, int* value, int root);
  };

  // NOLINTNEXTLINE(performance-enum-size): C ABI boundary; `version` above is int
  enum
  {
    CATALYST_MPI_OPS_VERSION = 1
  };

  /** Entry point exported by `libcatalyst_mpi`; the table is owned by the module. */
  // NOLINTNEXTLINE(modernize-use-using,modernize-redundant-void-arg): C header
  typedef const struct catalyst_mpi_ops* (*catalyst_mpi_ops_fn)(void);

#define CATALYST_MPI_OPS_SYMBOL "catalyst_mpi_get_ops"

  /** Returns 0 on success, having reported any failure on stderr. */
  int catalyst_mpi_load(void);

  /** NULL when built without MPI, or before a successful `catalyst_initialize`. */
  const struct catalyst_mpi_ops* catalyst_mpi_get_loaded_ops(void);

#ifdef __cplusplus
}
#endif

#endif
