/*
 * Distributed under OSI-approved BSD 3-Clause License. See
 * accompanying License.txt
 */
#ifndef catalyst_python_tools_h
#define catalyst_python_tools_h

#if CATALYST_WRAP_PYTHON
#include "Python.h"
#else
using PyObject = struct
{
  int x;
};
#endif

// include conduit C API.
#include <conduit.h>

#include <catalyst_export.h>

#ifdef __cplusplus
extern "C"
{
#endif
  // Wrap a conduit node for python. This is indented for passing a node from
  // C/C++ to python if owns == 1 the ownership of the node passes to Python
  // and thus upon descrution of the PyObject the node is freed.
  CATALYST_EXPORT PyObject* PyCatalystConduit_Node_Wrap(conduit_node* node, int owns);
#ifdef __cplusplus
} // extern "C"
#endif

#endif // #ifndef catalyst_python_tools_h
