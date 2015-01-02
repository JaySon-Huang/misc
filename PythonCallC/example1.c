#include <stdio.h>
#include <stdlib.h>

#include <Python/Python.h>

int hello1()
{
    printf("Hello Python with C\n");
    return 0x123;
}

PyObject *hello2(PyObject *self, PyObject *args)
{
    // https://docs.python.org/2/c-api/tuple.html
    // https://docs.python.org/2/c-api/arg.html?highlight=py_buildvalue#c.Py_BuildValue
    return Py_BuildValue("(iis)", 1, 2, "three");
}

PyObject *hello3(PyObject *self, PyObject *args)
{
    PyObject *list = PyList_New(0);
    PyObject *item = NULL;
    for (int i=0; i!=5; ++i){
        PyObject *sublist = PyList_New(0);
        for (int j=0; j!=64; ++j){
            PyList_Append(sublist, Py_BuildValue("i", i*100+j));
        }
        PyList_Append(list, sublist);
    }
    return list;
}
