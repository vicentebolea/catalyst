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
#include "conduit_python.hpp"   // PyConduit_Node_Python_Wrap
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
    else
    {
      CONDUIT_PYTHON_INITIALIZED = true;
    }
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
