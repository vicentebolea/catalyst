/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

/**
 * Loads `libcatalyst_mpi` from the implementation directory. See
 * `catalyst_mpi_ops.h` for the boundary it provides.
 */

#include "catalyst_mpi_ops.h"
#include "catalyst_search_path.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#define CATALYST_MPI_LIBNAME "catalyst_mpi.dll"
#define CATALYST_MPI_DIRSEP "\\"
#else
#include <dlfcn.h>
#define CATALYST_MPI_LIBNAME "libcatalyst_mpi.so"
#define CATALYST_MPI_DIRSEP "/"
#endif

static const struct catalyst_mpi_ops* loaded_ops = NULL;

int catalyst_mpi_load(void)
{
  if (loaded_ops)
  {
    return 0;
  }

  char* dir = catalyst_default_search_path();
  if (!dir)
  {
    fprintf(
      stderr, "catalyst: could not locate libcatalyst; cannot load " CATALYST_MPI_LIBNAME "\n");
    return 1;
  }

  size_t len = strlen(dir) + strlen(CATALYST_MPI_DIRSEP) + strlen(CATALYST_MPI_LIBNAME) + 1;
  char* full = (char*)malloc(len);
  if (!full)
  {
    free(dir);
    return 1;
  }
  snprintf(full, len, "%s" CATALYST_MPI_DIRSEP CATALYST_MPI_LIBNAME, dir);
  free(dir);

#ifdef _WIN32
  // The path is UTF-8; the ANSI entry point would read it in the active code page.
  HMODULE handle = NULL;
  const int wide_len = MultiByteToWideChar(CP_UTF8, 0, full, -1, NULL, 0);
  if (wide_len > 0)
  {
    wchar_t* wide_full = (wchar_t*)malloc((size_t)wide_len * sizeof(wchar_t));
    if (wide_full)
    {
      MultiByteToWideChar(CP_UTF8, 0, full, -1, wide_full, wide_len);
      handle = LoadLibraryExW(wide_full, NULL, 0);
      free(wide_full);
    }
  }
#else
  // RTLD_GLOBAL so the implementation loaded afterwards sees these symbols.
  void* handle = dlopen(full, RTLD_LAZY | RTLD_GLOBAL);
#endif

  if (!handle)
  {
    fprintf(stderr,
      "catalyst: Catalyst was built with MPI support but its MPI module could not be loaded "
      "from `%s`. Catalyst cannot determine rank or synchronize without it.\n",
      full);
    free(full);
    return 1;
  }

#ifdef _WIN32
  catalyst_mpi_ops_fn get_ops =
    (catalyst_mpi_ops_fn)GetProcAddress(handle, CATALYST_MPI_OPS_SYMBOL);
#else
  catalyst_mpi_ops_fn get_ops = (catalyst_mpi_ops_fn)dlsym(handle, CATALYST_MPI_OPS_SYMBOL);
#endif

  if (!get_ops)
  {
    fprintf(stderr, "catalyst: `%s` does not export `%s`\n", full, CATALYST_MPI_OPS_SYMBOL);
    free(full);
    return 1;
  }

  const struct catalyst_mpi_ops* ops = get_ops();
  if (!ops || ops->version != CATALYST_MPI_OPS_VERSION)
  {
    fprintf(stderr,
      "catalyst: `%s` provides MPI operations version %d; this libcatalyst requires %d\n", full,
      ops ? ops->version : -1, CATALYST_MPI_OPS_VERSION);
    free(full);
    return 1;
  }

  if (!ops->is_usable || !ops->comm_rank || !ops->comm_size || !ops->allreduce_min_int ||
    !ops->bcast_int || !ops->comm_dup || !ops->comm_free || !ops->query_thread_multiple)
  {
    fprintf(stderr,
      "catalyst: `%s` reports MPI operations version %d but does not provide every operation\n",
      full, ops->version);
    free(full);
    return 1;
  }

  free(full);
  loaded_ops = ops;
  return 0;
}

const struct catalyst_mpi_ops* catalyst_mpi_get_loaded_ops(void)
{
  return loaded_ops;
}
