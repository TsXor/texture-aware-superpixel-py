# cython:language_level=3
'''Not used in recalcAngle. You may use it in python?'''
cimport cython
from cpython.mem cimport PyMem_Malloc, PyMem_Realloc, PyMem_Free

import numpy as np
cimport numpy as cnp
from libc.math cimport floor as cfloor
from libc.math cimport ceil as cceil
from libc.math cimport sin as csin
from libc.math cimport cos as ccos
from .mathutils cimport square

@cython.boundscheck(False)
@cython.wraparound(False)
@cython.nonecheck(False)
cdef void _calcLocalBilinearInterpolation(int j, int i, double alpha, double [:, :] I, int radius, double [:, :] window) nogil:
    cdef double x, y, p1, p2, p3, p4, s, t
    cdef int a, b

    cdef int m, n
    for n in range(j-radius, j+radius+1):
        for m in range(i-radius, i+radius+1):
            if square(i-m) + square(j-n) <= square(radius):
                y = (n-j) * ccos(alpha) - (m-i) * csin(alpha) + j
                x = (n-j) * csin(alpha) + (m-i) * ccos(alpha) + i
                p1 = I[<int>cfloor(y), <int>cfloor(x)]
                p2 = I[<int>cfloor(y), <int>cceil(x) ]
                p3 = I[<int>cceil(y) , <int>cfloor(x)]
                p4 = I[<int>cceil(y) , <int>cceil(x) ]
                s = x - cfloor(x)
                t = y - cfloor(y)
                b = n - j + radius + 1
                a = m - i + radius + 1
                window[b, a] = (1-s) * (1-t) * p1 \
                             +   s   * (1-t) * p2 \
                             + (1-s) *   t   * p3 \
                             +   s   *   t   * p4

cpdef double[:, :] calcLocalBilinearInterpolation(int j, int i, double alpha, double [:, :] I, int radius, double [:, :] window=None):
    cdef int k = 2 * radius + 1

    cdef double* mem
    if window is None:
        mem = <double *>PyMem_Malloc(k * k * sizeof(double))
        if not mem: raise MemoryError()
        window = <double[:k, :k]>mem

    _calcLocalBilinearInterpolation(j, i, alpha, I, radius, window)
    return window
                                