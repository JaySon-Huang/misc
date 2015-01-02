#include <stdio.h>
#include <stdlib.h>

#include <Python/Python.h>

PyObject *hello4(PyObject *self, PyObject *args)
{
    PyObject *dict = PyDict_New();

    PyObject *list = PyList_New(4640);
    int cnt=0;
    for (int i=0; i!=4640; ++i){
        PyObject *sublist = PyList_New(64);
        for (int j=0; j!=64; ++j){
            PyList_SetItem(sublist, j, Py_BuildValue("i", i*100+j));
        }
        printf("%d\n", cnt++);
        PyList_SetItem(list, i, sublist);
    }
    PyDict_SetItem(dict, Py_BuildValue("s", "Y"), list);

    return dict;
}
