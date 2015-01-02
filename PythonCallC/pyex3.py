import ctypes
so = ctypes.CDLL("./libhello.so")
hello4func = so.hello4
hello4func.restype = ctypes.py_object
ret = hello4func()
print ret
