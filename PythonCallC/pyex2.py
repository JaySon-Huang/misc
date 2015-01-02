import ctypes
so = ctypes.CDLL("./libjpegtbx.so")
getdct = so.getdct
getdct.restype = ctypes.py_object
ret = getdct()
print ret
