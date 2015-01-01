### example1.c
    
    >>> gcc example1.c -fPIC -shared -o libhello.so -framework Python
    >>> python pyex.py

result:

    Hello Python with C
    0x123
    (1, 2, 'three')
