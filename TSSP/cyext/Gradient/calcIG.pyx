# cython:language_level=3
cimport cython
from cython.parallel import prange
from libc.stdlib cimport malloc, free
from cpython.mem cimport PyMem_Malloc, PyMem_Realloc, PyMem_Free

import numpy as np
cimport numpy as cnp
from libc.math cimport exp as cexp
from .mathutils cimport square

from .calcAngle cimport calcAngle
from .recalcAngle cimport recalcAngle


@cython.boundscheck(False)
@cython.wraparound(False)
@cython.cdivision(True)
cdef void _calcIG(double[:, :] I, int radius, double[:, :] ig, double[:, :] alpha):
    cdef int k = 2 * radius + 1
    cdef int i = radius + 1
    cdef double sigma = radius / 2.0
    cdef double *weight_mem = <double *>malloc(k * k * sizeof(double))
    cdef double[:, :] weight = <double[:k, :k]>weight_mem
    
    cdef int x, y
    cdef int distance
    cdef double w
    with nogil:
        for y in prange(i):
            for x in prange(i):
                distance = square(radius-y) + square(radius-x)
                w = cexp(-(<double>distance)/(2*square(sigma))) if distance <= square(radius) else 0
                weight[         y,          x] = w
                weight[2*radius-y,          x] = w
                weight[         y, 2*radius-x] = w
                weight[2*radius-y, 2*radius-x] = w
    
    calcAngle(I, radius, weight, alpha)
    #recalcAngle(I, radius, weight, alpha, ig, betta)
    recalcAngle(I, radius, weight, alpha, ig)

    free(weight_mem)

cpdef tuple calcIG(double[:, :] I, int radius, double[:, :] ig=None, double[:, :] alpha=None):
    cdef int h = <int>I.shape[0]
    cdef int w = <int>I.shape[1]

    cdef double* ig_mem
    if ig is None:
        ig_mem = <double *>PyMem_Malloc(h * w * sizeof(double))
        if not ig_mem: raise MemoryError()
        ig = <double[:h, :w]>ig_mem

    cdef double* alpha_mem
    if alpha is None:
        alpha_mem = <double *>PyMem_Malloc(h * w * sizeof(double))
        if not alpha_mem: raise MemoryError()
        alpha = <double[:h, :w]>alpha_mem
    
    _calcIG(I, radius, ig, alpha)

    return ig, alpha