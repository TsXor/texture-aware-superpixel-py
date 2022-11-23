# cython:language_level=3
cimport cython

import numpy as np
cimport numpy as cnp
from .calcIG cimport calcIG

@cython.boundscheck(False)
@cython.wraparound(False)
cpdef tuple HybridGradient(unsigned char[:, :, :] uimg, int radius):
    cdef list igrad
    cdef cnp.ndarray[double, ndim=3] igradx, igrady
    cdef cnp.ndarray[double, ndim=2] Ig1, Ig2, Amptitude, Xita
    cdef double[:, :] IgR, XitaR, IgG, XitaG, IgB, XitaB

    cdef cnp.ndarray[double, ndim=3] img = np.asarray(uimg) / 255

    igrad = np.gradient(img/255, axis=(0, 1))
    igradx = igrad[1]; igrady = igrad[0]
    Ig1 = np.sqrt(np.sum(np.square(igradx) + np.square(igrady), axis=2))
    Ig1 = (Ig1 - np.min(Ig1)) / (np.max(Ig1) - np.min(Ig1))

    IgR, XitaR = calcIG(img[:, :, 0], radius);
    IgG, XitaG = calcIG(img[:, :, 1], radius);
    IgB, XitaB = calcIG(img[:, :, 2], radius);
    
    Ig2 = np.sqrt(np.square(IgR) + np.square(IgG) + np.square(IgB))
    if np.max(Ig2) == np.min(Ig2): raise ValueError('Do not gave me an image of one pure color, that just makes no sense.')
    Ig2 = (Ig2 - np.min(Ig2)) / (np.max(Ig2) - np.min(Ig2))
    Xita = (np.asarray(XitaR) + np.asarray(XitaG) + np.asarray(XitaB)) / 3
    Amptitude = Ig1 * Ig2
    Amptitude = (Amptitude - np.min(Amptitude)) / (np.max(Amptitude) - np.min(Amptitude)) * 255
    Xita = np.pad(Xita, radius, 'symmetric')

    return Amptitude, Xita