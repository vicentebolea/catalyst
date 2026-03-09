/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

// NOLINTNEXTLINE(bugprone-reserved-identifier)
#define _GNU_SOURCE

#include "catalyst_api.h"
#include "catalyst_async.h"
#include "catalyst_impl.h"
#include "catalyst_version.h"

#include "conduit_node.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
typedef HMODULE catalyst_handle_t;
#define PATH_SEPARATOR ';'
#else
#include <dlfcn.h>
typedef void* catalyst_handle_t;
#define PATH_SEPARATOR ':'
#endif

static char* default_search_path();
static catalyst_handle_t handle_default();
static catalyst_handle_t handle_open(const char* directory, const char* libname);
static void* handle_load_symbol(catalyst_handle_t handle, const char* symbol);
static int handle_is_valid(catalyst_handle_t handle);

static struct catalyst_impl const* impl = NULL;
static int debug_flag = -1;
extern struct catalyst_impl const default_impl;

// Store implementation metadata for catalyst_about
static char* loaded_impl_path = NULL;
static const char* loaded_impl_source = NULL; // "environment", "params", "default", "stub"

#define catalyst_debug(fmt, ...)                                                                   \
  do                                                                                               \
  {                                                                                                \
    if (debug_flag)                                                                                \
    {                                                                                              \
      fprintf(stderr, "catalyst debug: " fmt, ##__VA_ARGS__);                                      \
    }                                                                                              \
  } while (0)

static catalyst_handle_t handle_from_env(const char* impl_name)
{
  catalyst_handle_t handle = handle_default();

  const char* search_env = getenv("CATALYST_IMPLEMENTATION_PATHS");
  if (search_env)
  {
    char* paths = strdup(search_env);
    catalyst_debug("search paths from `CATALYST_IMPLEMENTATION_PATHS`\n");
    if (paths)
    {
      char* pathsep = paths;
      char* curpath = paths;
      while (*pathsep)
      {
        int newpath = 0;
        if (*pathsep == PATH_SEPARATOR)
        {
          *pathsep = '\0';
        }
        if (!*pathsep)
        {
          newpath = 1;
        }

        if (newpath)
        {
          handle = handle_open(curpath, impl_name);
          if (handle_is_valid(handle))
          {
            // Record that implementation was found via environment variable
            loaded_impl_source = "environment";
            curpath = NULL;
            break;
          }
        }

        ++pathsep;
        if (newpath)
        {
          curpath = pathsep;
        }
      }

      if (!handle_is_valid(handle) && curpath)
      {
        handle = handle_open(curpath, impl_name);
        if (handle_is_valid(handle))
        {
          loaded_impl_source = "environment";
        }
      }
    }

    free(paths);
  }

  return handle;
}

static enum catalyst_status catalyst_load(const conduit_node* params)
{
  // Ensure that an implementation has not already been loaded.
  if (impl)
  {
    return catalyst_status_error_already_loaded;
  }

  // C APIs all expect non-const pointers.
  conduit_node* p = (conduit_node*)params;

  const char* prefer_env = getenv("CATALYST_IMPLEMENTATION_PREFER_ENV");
  int should_prefer_env = prefer_env && *prefer_env;
  catalyst_debug("preferring environment variables? %s\n", should_prefer_env ? "yes" : "no");

  char* impl_name = NULL;

  if (should_prefer_env)
  {
    impl_name = getenv("CATALYST_IMPLEMENTATION_NAME");
    catalyst_debug("implementation name from `CATALYST_IMPLEMENTATION_NAME`: %s\n",
      impl_name ? impl_name : "(none)");
  }

  if (!impl_name && conduit_node_has_path(p, "catalyst_load/implementation"))
  {
    impl_name = conduit_node_fetch_path_as_char8_str(p, "catalyst_load/implementation");
    catalyst_debug("implementation name from `catalyst_load/implementation`: %s\n",
      impl_name ? impl_name : "(none)");
  }

  if (!impl_name && !should_prefer_env)
  {
    impl_name = getenv("CATALYST_IMPLEMENTATION_NAME");
    catalyst_debug("implementation name from `CATALYST_IMPLEMENTATION_NAME`: %s\n",
      impl_name ? impl_name : "(none)");
  }

  if (impl_name)
  {
    catalyst_handle_t handle = handle_default();

    if (should_prefer_env)
    {
      handle = handle_from_env(impl_name);
    }

    if (!handle_is_valid(handle) && conduit_node_has_path(p, "catalyst_load/search_paths"))
    {
      conduit_node* search_paths = conduit_node_fetch(p, "catalyst_load/search_paths");
      if (search_paths)
      {
        conduit_index_t npaths = conduit_node_number_of_children(search_paths);
        for (conduit_index_t i = 0; i < npaths; ++i)
        {
          conduit_node* path_node = conduit_node_child(search_paths, i);
          char* search_path = conduit_node_as_char8_str(path_node);
          catalyst_debug("search path from `catalyst_load/search_paths`: `%s`\n",
            search_path ? search_path : "(null)");
          if (search_path && *search_path)
          {
            handle = handle_open(search_path, impl_name);
            if (handle_is_valid(handle))
            {
              // Record that implementation was found via params
              loaded_impl_source = "params";
              break;
            }
          }
        }
      }
    }

    if (!handle_is_valid(handle) && !should_prefer_env)
    {
      handle = handle_from_env(impl_name);
    }

    // Search the default path
    if (!handle_is_valid(handle))
    {
      char* beside_catalyst = default_search_path();
      char* default_paths[] = {
        beside_catalyst,
        NULL,
      };
      for (char** search_path = &default_paths[0]; *search_path; ++search_path)
      {
        catalyst_debug("search path from default list: `%s`\n", *search_path);
        handle = handle_open(*search_path, impl_name);
        if (handle_is_valid(handle))
        {
          // Record that implementation was found via default search path
          loaded_impl_source = "default";
          break;
        }
      }
      free(beside_catalyst);
    }

    if (!handle_is_valid(handle))
    {
      return catalyst_status_error_not_found;
    }

    impl = (struct catalyst_impl const*)handle_load_symbol(handle, "catalyst_api_impl");
    catalyst_debug("loaded implementation: %p\n", impl);
  }
  else
  {
    impl = &default_impl;
    catalyst_debug("no implementation named; using the `stub` implementation: %p\n", impl);
    // Record that we're using the built-in stub implementation
    free(loaded_impl_path);
    loaded_impl_path = strdup("(stub)");
    loaded_impl_source = "stub";
  }

  if (!impl)
  {
    return catalyst_status_error_not_catalyst;
  }

  if (impl->version != 1 && impl->version != 2)
  {
    return catalyst_status_error_unsupported_version;
  }

  // Ensure that all required API functions are provided.
  if (!impl->initialize || !impl->execute || !impl->finalize || !impl->about || !impl->results)
  {
    impl = NULL;
    return catalyst_status_error_incomplete;
  }

  // Check that the conduit is a potential match.
  conduit_uint64 conduit_is_external = impl->version >= 2
    ? impl->conduit_is_external
    : 0; // version < 2 never supported an external conduit
  if (conduit_is_external != CATALYST_WITH_EXTERNAL_CONDUIT)
  {
    return catalyst_status_error_conduit_mismatch;
  }

  return catalyst_status_ok;
}

#define impl_check(method)                                                                         \
  do                                                                                               \
  {                                                                                                \
    if (!impl)                                                                                     \
    {                                                                                              \
      return catalyst_status_error_no_implementation;                                              \
    }                                                                                              \
    if (!impl->method)                                                                             \
    {                                                                                              \
      return catalyst_status_error_no_implementation;                                              \
    }                                                                                              \
  } while (0)

enum catalyst_status catalyst_initialize(const conduit_node* params)
{
  if (debug_flag < 0)
  {
    char* debug_env = getenv("CATALYST_DEBUG");
    debug_flag = debug_env && *debug_env;
  }

  if (!impl)
  {
    enum catalyst_status err = catalyst_load(params);
    if (err != catalyst_status_ok)
    {
      return err;
    }
  }

  impl_check(initialize);
  enum catalyst_status status = (*impl->initialize)(params);

  if (status == catalyst_status_ok)
  {
    // Initialize async support (will check if enabled via params/env)
    catalyst_async_initialize(params);
  }

  return status;
}

enum catalyst_status catalyst_execute(const conduit_node* params)
{
  impl_check(execute);

  // Route through async if enabled (falls back to sync internally if not)
  return catalyst_async_execute(params, impl->execute);
}

enum catalyst_status catalyst_finalize(const conduit_node* params)
{
  // Finalize async first (flushes pending work)
  catalyst_async_finalize(params);

  impl_check(finalize);
  return (*impl->finalize)(params);
}

enum catalyst_status catalyst_about(conduit_node* params)
{
  impl_check(about);
  catalyst_async_impl_lock();
  enum catalyst_status status = (*impl->about)(params);
  catalyst_async_impl_unlock();

  // Add implementation info if not already populated by the backend
  if (status == catalyst_status_ok)
  {
    // Add libcatalyst runtime version
    if (!conduit_node_has_path(params, "catalyst/version"))
    {
      conduit_node_set_path_char8_str(params, "catalyst/version", CATALYST_VERSION);
    }

    // Add implementation path
    if (loaded_impl_path)
    {
      if (conduit_node_has_path(params, "catalyst/implementation_path"))
      {
        catalyst_debug("warning: catalyst/implementation_path already set by backend\n");
      }
      else
      {
        conduit_node_set_path_char8_str(params, "catalyst/implementation_path", loaded_impl_path);
      }
    }

    // Add implementation source
    if (loaded_impl_source)
    {
      if (conduit_node_has_path(params, "catalyst/implementation_source"))
      {
        catalyst_debug("warning: catalyst/implementation_source already set by backend\n");
      }
      else
      {
        conduit_node_set_path_char8_str(
          params, "catalyst/implementation_source", loaded_impl_source);
      }
    }

    // Add implementation ABI version
    if (impl && !conduit_node_has_path(params, "catalyst/implementation_abi_version"))
    {
      conduit_node_set_path_int32(
        params, "catalyst/implementation_abi_version", (int)impl->version);
    }

    // Add async execution info
    catalyst_async_populate_about(params);
  }

  return status;
}

enum catalyst_status catalyst_results(conduit_node* params)
{
  impl_check(results);
  catalyst_async_impl_lock();
  enum catalyst_status status = (*impl->results)(params);
  catalyst_async_impl_unlock();
  return status;
}

#ifdef _WIN32
char* default_search_path()
{
  MEMORY_BASIC_INFORMATION mbi;
  VirtualQuery(catalyst_initialize, &mbi, sizeof(mbi));
  wchar_t pathBuf[16384];
  if (!GetModuleFileNameW((HMODULE)mbi.AllocationBase, pathBuf, sizeof(pathBuf)))
  {
    return NULL;
  }

  size_t size = WideCharToMultiByte(CP_UTF8, 0, pathBuf, -1, NULL, 0, NULL, NULL);
  char* path_utf8 = (char*)malloc(size);
  if (!path_utf8)
  {
    return NULL;
  }
  path_utf8[0] = '\0';
  WideCharToMultiByte(CP_UTF8, 0, pathBuf, -1, path_utf8, size, NULL, NULL);

  // The Windows API will always use `\` separators here.
  char* dirsep = strrchr(path_utf8, '\\');
  *dirsep = '\0';

  size_t dirlen = strlen(path_utf8) + 9 + 1;
  char* directory_name = (char*)malloc(dirlen);
  if (!directory_name)
  {
    return NULL;
  }

  snprintf(directory_name, dirlen, "%s/catalyst", path_utf8);
  free(path_utf8);

  return directory_name;
}

catalyst_handle_t handle_default()
{
  return NULL;
}

catalyst_handle_t handle_open(const char* directory, const char* libname)
{
  size_t path_len = strlen(directory) + strlen(libname) + 9 + 5 + 1 + strlen(CATALYST_DEBUG_SUFFIX);
  char* full_library_path = (char*)malloc(path_len);
  if (!full_library_path)
  {
    return NULL;
  }

  snprintf(
    full_library_path, path_len, "%s/catalyst-%s" CATALYST_DEBUG_SUFFIX ".dll", directory, libname);

  catalyst_handle_t handle = LoadLibraryExA(full_library_path, NULL, 0);

  LPSTR errstring = NULL;
  const char* noerror = "(no error)";
  if (debug_flag && !handle_is_valid(handle))
  {
    DWORD const err_code = GetLastError();

    DWORD const ret = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, err_code, 0, (LPSTR)&errstring, 0, NULL);

    if (!ret)
    {
      catalyst_debug("failed to get error string from system");
      errstring = NULL;
      noerror = "(failed to get error string)";
    }
  }

  catalyst_debug("trying to load `%s`: %s (%s)\n", full_library_path,
    handle_is_valid(handle) ? "valid" : "invalid", errstring ? errstring : noerror);

  if (errstring)
  {
    LocalFree(errstring);
  }

  // Store the path if loading succeeded
  if (handle_is_valid(handle))
  {
    free(loaded_impl_path);
    loaded_impl_path = full_library_path;
  }
  else
  {
    free(full_library_path);
  }
  return handle;
}

void* handle_load_symbol(catalyst_handle_t handle, const char* symbol)
{
  return (void*)GetProcAddress(handle, symbol);
}

int handle_is_valid(catalyst_handle_t handle)
{
  return handle != NULL;
}
#else
char* default_search_path()
{
  catalyst_handle_t handle = dlsym(RTLD_DEFAULT, "catalyst_initialize");
  if (!handle)
  {
    return NULL;
  }

  Dl_info info;
  int ret = dladdr(handle, &info);
  if (ret == 0 || !info.dli_saddr || !info.dli_fname)
  {
    return NULL;
  }

  char* fname = strdup(info.dli_fname);
  if (!fname)
  {
    return NULL;
  }

  char* dirsep = strrchr(fname, '/');
  *dirsep = '\0';

  size_t dirlen = strlen(fname) + 9 + 1;
  char* directory_name = (char*)malloc(dirlen);
  if (!directory_name)
  {
    free(fname);
    return NULL;
  }

  snprintf(directory_name, dirlen, "%s/catalyst", fname);
  free(fname);

  return directory_name;
}

catalyst_handle_t handle_default()
{
  return NULL;
}

catalyst_handle_t handle_open(const char* directory, const char* libname)
{
  size_t path_len = strlen(directory) + strlen(libname) + 9 + 7 + 1;
  char* full_library_path = (char*)malloc(path_len);
  if (!full_library_path)
  {
    return NULL;
  }

  snprintf(full_library_path, path_len, "%s/libcatalyst-%s.so", directory, libname);

#ifdef __APPLE__
  int scope_flag = RTLD_LOCAL;
#else
  int scope_flag = RTLD_GLOBAL;
#endif

  catalyst_handle_t handle = dlopen(full_library_path, RTLD_LAZY | scope_flag);
  if (!handle)
  {
    printf("failed to open library: %s\n", dlerror());
  }
  char* errstring = NULL;
  if (!handle_is_valid(handle))
  {
    errstring = dlerror();
  }
  catalyst_debug("trying to load `%s`: %s (%s)\n", full_library_path,
    handle_is_valid(handle) ? "valid" : "invalid", errstring ? errstring : "(no error)");

  // Store the path if loading succeeded
  if (handle_is_valid(handle))
  {
    free(loaded_impl_path);
    loaded_impl_path = full_library_path;
  }
  else
  {
    free(full_library_path);
  }
  return handle;
}

void* handle_load_symbol(catalyst_handle_t handle, const char* symbol)
{
  return dlsym(handle, symbol);
}

int handle_is_valid(catalyst_handle_t handle)
{
  return handle != NULL;
}
#endif
