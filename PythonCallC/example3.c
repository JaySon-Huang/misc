#include <stdio.h>
#include <stdlib.h>

#include <Python/Python.h>

#include <assert.h>

#define LIST_SIZE 1000

PyObject *hello4(PyObject *self, PyObject *args)
{
    PyObject *dict = PyDict_New();
    assert(dict != NULL);
    printf("dict is NOT NULL.\n");
    PyObject *list = PyList_New(LIST_SIZE);
    assert(list != NULL);
    printf("list is NOT NULL.\n");
    int cnt=0;
    PyObject *sublist;
    for (int i=0; i!=LIST_SIZE; ++i){
        printf("Creating new sublist...\n");
        sublist = PyList_New(64);
        assert(sublist != NULL);
        printf("sublist is NOT NULL.\n");
        for (int j=0; j!=64; ++j){
            PyObject *val = Py_BuildValue("i", i*100+j);
            PyList_SetItem(sublist, j, val);
        }
        printf("%d\n", cnt++);
        PyList_SetItem(list, i, sublist);

        printf("sublist added to list\n");
    }
    PyDict_SetItem(dict, Py_BuildValue("s", "Y"), list);

    return dict;
}
