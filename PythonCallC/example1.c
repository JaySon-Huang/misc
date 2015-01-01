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
    return Py_BuildValue("(iis)", 1, 2, "three");
}
