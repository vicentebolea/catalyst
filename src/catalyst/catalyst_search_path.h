/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

#ifndef catalyst_search_path_h
#define catalyst_search_path_h

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * The directory modules are loaded from: `catalyst/` beside `libcatalyst`.
   * Caller frees. Not static because the MPI loader uses it too; hidden
   * visibility keeps it out of the ABI.
   */
  char* catalyst_default_search_path(void);

#ifdef __cplusplus
}
#endif

#endif
