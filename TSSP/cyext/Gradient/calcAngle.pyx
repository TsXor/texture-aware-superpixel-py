# cython:language_level=3
cimport cython
from cython.parallel import prange
from libc.stdlib cimport malloc, free
from cpython.mem cimport PyMem_Malloc, PyMem_Realloc, PyMem_Free

import numpy as np
cimport numpy as cnp
from libc.math cimport atan as catan
from libc.math cimport pi as cpi
from .mathutils cimport madd2d, msub2d, mmul2d, mdiv2d, msum2d


@cython.boundscheck(False)
@cython.wraparound(False)
@cython.cdivision(True)
@cython.nonecheck(False)
cdef void _calcAngle(double[:, :] Ip, int h, int w, int radius, double[:, :] weight, double[:, :] alpha) nogil:
    cdef int k = 2 * radius + 1
    
    cdef int x, xi, cx, xmin, xmax, y, yi, cy, ymin, ymax
    cdef double igx, igy
    # l - left, r - right, u - up, d - down
    cdef double plu, pru, pld, prd
    for y in prange(h):
        for x in prange(w):
            ymin = y; ymax = y + 2 * radius; cy = y + radius
            xmin = x; xmax = x + 2 * radius; cx = x + radius
            
            igx = 0; igy = 0
            for yi in range(y, cy+1):
                for xi in range(x, cx+1):
                    if weight[yi-ymin, xi-xmin] == 0.0:
                        continue
                    plu = Ip[     yi,      xi] * weight[yi-ymin, xi-xmin]
                    pru = Ip[     yi, 2*cx-xi] * weight[yi-ymin, xmax-xi]
                    pld = Ip[2*cy-yi,      xi] * weight[ymax-yi, xi-xmin]
                    prd = Ip[2*cy-yi, 2*cx-xi] * weight[ymax-yi, xmax-xi]
                    igx = igx + (pru + prd - plu - pld)
                    igy = igy + (pld + prd - plu - pru)
            
            alpha[y, x] = -catan(igy / igx) if igx != 0.0 else cpi/2

cpdef double[:, :] calcAngle(double[:, :] I, int radius, double[:, :] weight, double[:, :] alpha=None):
    cdef int h = <int>I.shape[0]
    cdef int w = <int>I.shape[1]
    cdef double[:, :] Ip = np.pad(I, radius, 'symmetric')

    cdef double* mem
    if alpha is None:
        mem = <double *>PyMem_Malloc(h * w * sizeof(double))
        if not mem: raise MemoryError()
        alpha = <double[:h, :w]>mem

    _calcAngle(Ip, h, w, radius, weight, alpha)
    
    return alpha
