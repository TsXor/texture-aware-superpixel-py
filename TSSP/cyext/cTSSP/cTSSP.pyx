# cython:language_level=3
cdef extern from "./TSSP.h":
    ctypedef size_t mwSize

    ctypedef struct TSSP_result_t:
        int nlabels
        int *labels
    
    TSSP_result_t* TSSPmain(int numsuper, mwSize h, mwSize w,
                            unsigned char *img, double *img_lab, double *img_gray,
                            double *magnitude, double *direction,
                            int radius, int gamma)

cimport cython
import numpy as np
cimport numpy as cnp
import cv2

@cython.boundscheck(False)
@cython.wraparound(False)
@cython.cdivision(True)
cdef tuple cTSSPmain(int region_size, int radius, int gamma,
                     unsigned char[:, :, :] img, double[:, :] magnitude, double [:, :]direction):

    cdef int h = <int>img.shape[0]
    cdef int w = <int>img.shape[1]
    cdef int numsuper = (h * w) // region_size

    cdef cnp.ndarray[unsigned char, ndim=3] img_pad = np.pad(img, ((radius, radius), (radius, radius), (0, 0)), 'symmetric')
    cdef double[:, :, :] img_lab = cv2.cvtColor(img_pad, cv2.COLOR_RGB2LAB).astype(np.double)
    cdef double[:, :] img_gray = cv2.cvtColor(img_pad, cv2.COLOR_RGB2GRAY).astype(np.double)

    cdef unsigned char[::1] Cimg = np.ravel(img)
    cdef double[::1] Cimg_lab = np.ravel(img_lab)
    cdef double[::1] Cimg_gray = np.ravel(img_gray)
    cdef double[::1] Cmagnitude = np.ravel(magnitude)
    cdef double[::1] Cdirection = np.ravel(direction)
    
    cdef TSSP_result_t *result = TSSPmain(numsuper, h, w,
                                          &Cimg[0], &Cimg_lab[0], &Cimg_gray[0],
                                          &Cmagnitude[0], &Cdirection[0], radius, gamma)

    cdef int nlabels = result.nlabels
    cdef int[:, :] labels = <int[:h, :w]>(result.labels)
    return nlabels, labels