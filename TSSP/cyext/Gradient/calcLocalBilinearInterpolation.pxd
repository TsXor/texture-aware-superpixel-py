# cython:language_level=3
cpdef double[:, :] calcLocalBilinearInterpolation(int j, int i, double alpha, double [:, :] I, int radius, double [:, :] window=*)