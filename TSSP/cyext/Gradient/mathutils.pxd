# cython:language_level=3

ctypedef fused num_t:
    int
    double

cdef inline num_t square(num_t x) nogil:
    return x * x

cdef void madd2d(num_t[:, :] a, num_t[:, :] b, num_t[:, :] to) nogil

cdef void msub2d(num_t[:, :] a, num_t[:, :] b, num_t[:, :] to) nogil

cdef void mmul2d(num_t[:, :] a, num_t[:, :] b, num_t[:, :] to) nogil

cdef void mdiv2d(num_t[:, :] a, num_t[:, :] b, num_t[:, :] to) nogil

cdef num_t msum2d(num_t[:, :] n) nogil