#include <Python.h>

#include "conduit.hpp"          // for conduit::Node definition
#include "conduit_cpp_to_c.hpp" // for conduit::c_node
#include "conduit_python.hpp" // for conduit python api PyConduit_Node_Check, PyConduit_Node_Get_Node_Ptr

#include "catalyst_api.h" // catalyst API

#ifdef _WIN32
#define CATALYST_PYTHON_EXPORT __declspec(dllexport)
#else
#define CATALYST_PYTHON_EXPORT __attribute__((visibility("default")))
#endif

//---------------------------------------------------------------------------//
static PyObject* CatalystError = NULL;
// Custom Exceptions matching catalyst_statuses
static PyObject* NoImplementationError = NULL;
static PyObject* AlreadyLoadedError = NULL;
static PyObject* NotFoundError = NULL;
static PyObject* NotCatalystError = NULL;
static PyObject* IncompleteError = NULL;
static PyObject* UnsupportedVersionError = NULL;
static PyObject* ConduitMismatchError = NULL;

static PyObject* parse_status(enum catalyst_status status)
{
  switch (status)
  {
    case catalyst_status::catalyst_status_error_no_implementation:
      PyErr_SetString(NoImplementationError, "An implementation has no been loaded.");
      break;
    case catalyst_status::catalyst_status_error_already_loaded:
      PyErr_SetString(AlreadyLoadedError, "An implementation has already been loaded.");
      break;
    case catalyst_status::catalyst_status_error_not_found:
      PyErr_SetString(NotFoundError, "The implementation library was not found.");
      break;
    case catalyst_status::catalyst_status_error_not_catalyst:
      PyErr_SetString(NotCatalystError, "The library did not contain a Catalyst API structure.");
      break;
    case catalyst_status::catalyst_status_error_incomplete:
      PyErr_SetString(
        IncompleteError, "The library did not provide all of the required API functions.");
      break;
    case catalyst_status::catalyst_status_error_unsupported_version:
      PyErr_SetString(
        UnsupportedVersionError, "The library did not provide a supported version of the API.");
      break;
    case catalyst_status::catalyst_status_error_conduit_mismatch:
      PyErr_SetString(ConduitMismatchError,
        "The Conduit state for the implementation does not match Catalyst's Conduit state.");
      break;
    case catalyst_status::catalyst_status_ok:
      Py_RETURN_NONE;
    default:
      PyErr_Format(PyExc_ValueError, "Unknown catalyst status code %d", int(status));
  }
  return NULL;
}

//---------------------------------------------------------------------------//
static PyObject* PyCatalyst_initialize(PyObject* /* self */, PyObject* args)
{
  PyObject* py_node = NULL;
  if (!PyArg_ParseTuple(args, "O", &py_node))
  {
    Py_RETURN_NONE;
  }
  if (!PyConduit_Node_Check(py_node))
  {
    PyErr_SetString(PyExc_TypeError, "Argument should be of type conduit.node");
    Py_RETURN_NONE;
  }

  conduit::Node& cpp_node = *PyConduit_Node_Get_Node_Ptr(py_node);
  conduit_node* c_node = conduit::c_node(&cpp_node);
  catalyst_status status = catalyst_initialize(c_node);
  return parse_status(status);
}

//---------------------------------------------------------------------------//
static PyObject* PyCatalyst_execute(PyObject* /* self */, PyObject* args)
{
  PyObject* py_node = NULL;
  if (!PyArg_ParseTuple(args, "O", &py_node))
  {
    Py_RETURN_NONE;
  }
  if (!PyConduit_Node_Check(py_node))
  {
    PyErr_SetString(PyExc_TypeError, "Argument should be of type conduit.node");
    Py_RETURN_NONE;
  }

  conduit::Node& cpp_node = *PyConduit_Node_Get_Node_Ptr(py_node);
  conduit_node* c_node = conduit::c_node(&cpp_node);
  catalyst_status status = catalyst_execute(c_node);
  return parse_status(status);
}

//---------------------------------------------------------------------------//
static PyObject* PyCatalyst_finalize(PyObject* /* self */, PyObject* args)
{
  PyObject* py_node = NULL;
  if (!PyArg_ParseTuple(args, "O", &py_node))
  {
    Py_RETURN_NONE;
  }
  if (!PyConduit_Node_Check(py_node))
  {
    PyErr_SetString(PyExc_TypeError, "Argument should be of type conduit.node");
    Py_RETURN_NONE;
  }

  conduit::Node& cpp_node = *PyConduit_Node_Get_Node_Ptr(py_node);
  conduit_node* c_node = conduit::c_node(&cpp_node);
  catalyst_status status = catalyst_finalize(c_node);
  return parse_status(status);
}

//---------------------------------------------------------------------------//
static PyObject* PyCatalyst_about(PyObject* /* self */, PyObject* args)
{
  PyObject* py_node = NULL;
  if (!PyArg_ParseTuple(args, "O", &py_node))
  {
    Py_RETURN_NONE;
  }
  if (!PyConduit_Node_Check(py_node))
  {
    PyErr_SetString(PyExc_TypeError, "Argument should be of type conduit.node");
    Py_RETURN_NONE;
  }

  conduit::Node& cpp_node = *PyConduit_Node_Get_Node_Ptr(py_node);
  conduit_node* c_node = conduit::c_node(&cpp_node);
  catalyst_status status = catalyst_about(c_node);
  return parse_status(status);
}

//---------------------------------------------------------------------------//
static PyObject* PyCatalyst_results(PyObject* /* self */, PyObject* args)
{
  PyObject* py_node = NULL;
  if (!PyArg_ParseTuple(args, "O", &py_node))
  {
    Py_RETURN_NONE;
  }
  if (!PyConduit_Node_Check(py_node))
  {
    PyErr_SetString(PyExc_TypeError, "Argument should be of type conduit.node");
    Py_RETURN_NONE;
  }

  conduit::Node& cpp_node = *PyConduit_Node_Get_Node_Ptr(py_node);
  conduit_node* c_node = conduit::c_node(&cpp_node);
  catalyst_status status = catalyst_results(c_node);
  return parse_status(status);
}

// Python Module Method Defs
//---------------------------------------------------------------------------//
static PyMethodDef catalyst_python_methods[] =
  // clang-format off
{ 
 { "initialize",(PyCFunction)PyCatalyst_initialize, METH_VARARGS, "Intialize Catalyst" },
 { "execute",   (PyCFunction)PyCatalyst_execute, METH_VARARGS, "Execute per timestep" },
 { "finalize", (PyCFunction)PyCatalyst_finalize, METH_VARARGS, "Finalize Catalyst" },
 { "about",    (PyCFunction)PyCatalyst_about, METH_VARARGS, "Returns information about the catalyst library" },
 { "results", (PyCFunction)PyCatalyst_results, METH_VARARGS, "Get results from the catalyst library" },
 { NULL, NULL, 0, NULL } 
};
// clang-format on

static struct PyModuleDef catalyst_module =
  // clang-format off
{ 
   PyModuleDef_HEAD_INIT, 
  "catalyst",
  "Python Interface for Catalyst",
  -1, // # TODO allow subinterprenters
  catalyst_python_methods 
};
// clang-format on

#define CATALYST_STRINGIFY(x) #x
#define CATALYST_TOSTRING(x) CATALYST_STRINGIFY(x)
#define CATALYST_CREATE_EXCEPTION(module, object, base)                                            \
  do                                                                                               \
  {                                                                                                \
    object = PyErr_NewException("catalyst." CATALYST_TOSTRING(object), base, NULL);                \
    Py_XINCREF(object);                                                                            \
    if (PyModule_AddObject(module, CATALYST_TOSTRING(object), object) < 0)                         \
    {                                                                                              \
      Py_XDECREF(object);                                                                          \
      Py_CLEAR(object);                                                                            \
      Py_DECREF(module);                                                                           \
      return NULL;                                                                                 \
    }                                                                                              \
  } while (0)

extern "C"
{
  CATALYST_PYTHON_EXPORT PyObject* PyInit_catalyst_python(void)
  {
    PyObject* py_module = PyModule_Create(&catalyst_module);
    if (!py_module)
    {
      return NULL;
    }

    // base Exception class
    CATALYST_CREATE_EXCEPTION(py_module, CatalystError, NULL);
    CATALYST_CREATE_EXCEPTION(py_module, NoImplementationError, CatalystError);
    CATALYST_CREATE_EXCEPTION(py_module, AlreadyLoadedError, CatalystError);
    CATALYST_CREATE_EXCEPTION(py_module, NotFoundError, CatalystError);
    CATALYST_CREATE_EXCEPTION(py_module, NotCatalystError, CatalystError);
    CATALYST_CREATE_EXCEPTION(py_module, IncompleteError, CatalystError);
    CATALYST_CREATE_EXCEPTION(py_module, UnsupportedVersionError, CatalystError);
    CATALYST_CREATE_EXCEPTION(py_module, ConduitMismatchError, CatalystError);

    // setup for conduit python c api
    if (import_conduit() < 0)
    {
      return NULL;
    }
    return py_module;
  }
}
#undef CATALYST_TOSTRING
#undef CATALYST_STRINGIFY
#undef CATALYST_CREATE_EXCEPTION
#undef CATALYST_PYTHON_EXPORT
