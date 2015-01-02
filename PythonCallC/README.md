### example1.c
    
    >>> gcc example1.c -fPIC -shared -o libhello.so -framework Python
    >>> python pyex.py

result:

    Hello Python with C
    0x123
    (1, 2, 'three')

### example2.c
    
    >>> gcc example2.c -fPIC -shared -ljpeg -o libjpegtbx.so -framework Python
    >>> python pyex2.py

report error...

Thanks to :
> * [how to deal with the PyObject* from C++ in Python](http://stackoverflow.com/questions/20281372/how-to-deal-with-the-pyobject-from-c-in-python)
> * [用C语言扩展Python的功能](http://www.ibm.com/developerworks/cn/linux/l-pythc/)
