# cython:language_level=3
cimport cython

import numpy as np
cimport numpy as cnp
import cv2

from .Gradient.HybridGradient cimport HybridGradient
from .cTSSP.cTSSP cimport cTSSPmain

@cython.boundscheck(False)
@cython.wraparound(False)
@cython.cdivision(True)
cpdef TSSuperPixel(unsigned char[:, :, :] img, int region_size, int radius, int gamma):
    cdef tuple hgrad = HybridGradient(img, radius)
    cdef double[:, :] magnitude = hgrad[0]
    cdef double[:, :] direction = hgrad[1]
    cdef tuple result = cTSSPmain(region_size, radius, gamma, img, magnitude, direction)
    return result