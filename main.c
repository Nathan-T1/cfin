
#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include <stdio.h>

static PyObject* system_call(PyObject *self, PyObject *args){
    const char *command;
    int sts;

    if(!PyArg_ParseTuple(args, "s", &command)){
      return NULL;
    };
    sts = system(command);
    if(sts < 0){
       PyErr_SetString(PyExc_ValueError, "System Command Failed");
       return NULL;
    }

    return PyLong_FromLong(sts);
};
static PyMethodDef system_call_methods[] = {
    {"system_call", system_call, METH_VARARGS, "Execute Shell Command"}
};
static struct PyModuleDef systemModule = {
    PyModuleDef_HEAD_INIT,
    "cfin",   /* name of module */
    "cfin Module", /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    system_call_methods
};
PyMODINIT_FUNC PyInit_cfin(void){
    return PyModule_Create(&systemModule);
};