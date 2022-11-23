# cython:language_level=3
cdef tuple cTSSPmain(int region_size, int radius, int gamma,
                     unsigned char[:, :, :] img, double[:, :] magnitude, double [:, :]direction)