# cython:language_level=3
cimport cython
from cython.parallel import prange
from cpython.mem cimport PyMem_Malloc, PyMem_Realloc, PyMem_Free

import numpy as np
cimport numpy as cnp
from libc.math cimport sqrt as csqrt
from libc.math cimport floor as cfloor
from libc.math cimport ceil as cceil
from libc.math cimport sin as csin
from libc.math cimport cos as ccos
from .mathutils cimport square, madd2d, msub2d, mmul2d, mdiv2d, msum2d

# cx - center x, cy - center y
# BI - BilinearInterpolation
@cython.boundscheck(False)
@cython.wraparound(False)
@cython.nonecheck(False)
cdef inline double calcPointBI(int xi, int yi, int cx, int cy, double alpha, double[:, :] I) nogil:
    cdef float ymax = <float>I.shape[0] - 1
    cdef float xmax = <float>I.shape[1] - 1

    cdef double x, y, p1, p2, p3, p4, s, t
    cdef double result

    y = (yi-cy) * ccos(alpha) - (xi-cx) * csin(alpha) + cy
    if y < 0: y = 0
    elif y > ymax: y = ymax
    x = (yi-cy) * csin(alpha) + (xi-cx) * ccos(alpha) + cx
    if x < 0: x = 0
    elif x > xmax: x = xmax
    p1 = I[<int>cfloor(y), <int>cfloor(x)]
    p2 = I[<int>cfloor(y), <int>cceil(x) ]
    p3 = I[<int>cceil(y) , <int>cfloor(x)]
    p4 = I[<int>cceil(y) , <int>cceil(x) ]
    s = x - cfloor(x)
    t = y - cfloor(y)
    result = (1-s) * (1-t) * p1 \
           +   s   * (1-t) * p2 \
           + (1-s) *   t   * p3 \
           +   s   *   t   * p4
    
    return result



@cython.boundscheck(False)
@cython.wraparound(False)
@cython.cdivision(True)
@cython.nonecheck(False)
#cdef void _recalcAngle(double[:, :] Ip, int h, int w, int radius, double[:, :] weight, double[:, :] alpha, double[:, :] ig, double[:, :] theta):
cdef void _recalcAngle(double[:, :] Ip, int h, int w, int radius, double[:, :] weight, double[:, :] alpha, double[:, :] ig) nogil:
    cdef int k = 2 * radius + 1
    cdef double kw = msum2d(weight[:, :radius])

    cdef int x, xi, cx, xmin, xmax, y, yi, cy, ymin, ymax
    cdef double igx, igy, igx2, igy2, alphai
    # l - left, r - right, u - up, d - down
    cdef double plu, pru, pld, prd
    for y in prange(h):
        for x in prange(w):
            ymin = y; ymax = y + 2 * radius; cy = y + radius
            xmin = x; xmax = x + 2 * radius; cx = x + radius
            alphai = alpha[y, x]

            igx = 0; igy = 0
            for yi in range(y, cy+1):
                for xi in range(x, cx+1):
                    if weight[yi-ymin, xi-xmin] == 0.0:
                        continue
                    plu = calcPointBI(     yi,      xi, cx, cy, alphai, Ip) * weight[yi-ymin, xi-xmin]
                    pru = calcPointBI(     yi, 2*cx-xi, cx, cy, alphai, Ip) * weight[yi-ymin, xmax-xi]
                    pld = calcPointBI(2*cy-yi,      xi, cx, cy, alphai, Ip) * weight[ymax-yi, xi-xmin]
                    prd = calcPointBI(2*cy-yi, 2*cx-xi, cx, cy, alphai, Ip) * weight[ymax-yi, xmax-xi]
                    igx = igx + (pru + prd - plu - pld)
                    igy = igy + (pld + prd - plu - pru)

            #igx2 = igx * ccos(alphai) - igy * csin(alphai)
            #igy2 = igx * csin(alphai) + igy * ccos(alphai)

            #if igx == 0:
            #    theta[y, x] = cpi/2
            #else:
            #    theta[y, x] = -catan(igy2 / igx2)
            
            ig[y, x] = csqrt(square(igx) + square(igy)) / kw

cpdef double[:, :] recalcAngle(double[:, :] I, int radius, double[:, :] weight, double[:, :] alpha, double[:, :] ig=None):
    cdef int h = <int>I.shape[0]
    cdef int w = <int>I.shape[1]
    cdef double[:, :] Ip = np.pad(I, radius, 'symmetric')

    cdef double* mem
    if ig is None:
        mem = <double *>PyMem_Malloc(h * w * sizeof(double))
        if not mem: raise MemoryError()
        ig = <double[:h, :w]>mem

    _recalcAngle(Ip, h, w, radius, weight, alpha, ig)

    return ig
