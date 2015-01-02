#!/usr/bin/python

from pprint import pprint as pp

import ctypes
so = ctypes.CDLL("./libhello.so")
ret = so.hello1()
print hex(ret)
hellofunc = so.hello2
hellofunc.restype = ctypes.py_object
ret = hellofunc()
print ret
print dir(hellofunc)

hello3func = so.hello3
hello3func.restype = ctypes.py_object
ret = hello3func()
for l in ret:
    print l
