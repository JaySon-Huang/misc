#!/usr/bin/python

import ctypes
so = ctypes.CDLL("./libhello.so")
ret = so.hello1()
print hex(ret)
hellofunc = so.hello2
hellofunc.restype = ctypes.py_object
ret = hellofunc()
print ret
