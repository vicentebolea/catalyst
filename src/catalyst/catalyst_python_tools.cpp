/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */

#define catalyst_python_tools_impl // allow catalyst_python_tools.h
                                   // to be included without first
                                   // without errors so we can get
                                   // CATALYST_WRAP_PYTHON
#include "catalyst_python_tools.h"

#if CATALYST_WRAP_PYTHON
#include "Python.h"
#include "conduit.hpp"          // for conduit::Node
#include "conduit_cpp_to_c.hpp" // for conduit::c_node

#ifndef _WIN32
#include <dlfcn.h>

namespace
{
// Avoid linking a specific libpython; this symbol is looked up at runtime instead.
// See docs/runtime_linkage.rst.
void* catalyst_capsule_import(const char* name, int no_block)
{
  using PyCapsuleImportFn = void* (*)(const char*, int);
  auto* capsule_import =
    reinterpret_cast<PyCapsuleImportFn>(dlsym(RTLD_DEFAULT, "PyCapsule_Import"));
  if (!capsule_import)
  {
    fprintf(stderr,
      "catalyst: no Python interpreter found in this process; "
      "cannot wrap a conduit node for Python\n");
    return nullptr;
  }
  return capsule_import(name, no_block);
}
}

// Preprocessor rather than a wrapper so the capsule name keeps coming from whichever
// `conduit_python.hpp` is in use rather than being duplicated here.
#define PyCapsule_Import catalyst_capsule_import
#endif

#include "conduit_python.hpp" // PyConduit_Node_Python_Wrap, import_conduit

#ifndef _WIN32
#undef PyCapsule_Import
#endif
#endif

#if CATALYST_WRAP_PYTHON
namespace
{
bool initialize_conduit()
{
  static bool CONDUIT_PYTHON_INITIALIZED = false;
  if (!CONDUIT_PYTHON_INITIALIZED)
  {
    if (import_conduit() == -1)
    {
      fprintf(stderr, "Could not import conduit");
      return false;
    }
    CONDUIT_PYTHON_INITIALIZED = true;
  }
  return true;
}
}
#endif

catalyst_py_object* PyCatalystConduit_Node_Wrap(conduit_node* node, int owns)
{
#if CATALYST_WRAP_PYTHON
  if (initialize_conduit())
  {
    // NOLINTNEXTLINE(misc-const-correctness): clang bug? `const` fails to compile here.
    conduit::Node* cppNode = conduit::cpp_node(node);
    return PyConduit_Node_Python_Wrap(cppNode, owns);
  }
  return nullptr;
#else
  (void)(node);
  (void)(owns);
  return nullptr;
#endif
}
#undef catalyst_python_tools_impl
