cimport cython
from cython.parallel import prange
#from cpython.mem cimport PyMem_Malloc, PyMem_Realloc, PyMem_Free

cdef int int_inf = 0x3f3f3f3f
cdef float float_inf = float('inf')

@cython.boundscheck(False)
@cython.wraparound(False)
@cython.nonecheck(False)
cdef inline void mufunc2i1o2d(num_t[:, :] a, num_t[:, :] b, num_t (*f)(num_t, num_t) nogil, num_t[:, :] to) nogil:
    cdef int h = <int>a.shape[0]
    cdef int w = <int>a.shape[1]

    # This function is only used in C, so manage the memory myself ;)

    #cdef num_t* mem
    #if to is None:
    #    mem = <num_t *>PyMem_Malloc(h * w * sizeof(num_t))
    #    if not mem: raise MemoryError()
    #    to = <num_t[:h, :w]>mem

    cdef int x, y
    for y in prange(h):
        for x in prange(w):
            to[y, x] = f(a[y, x], b[y, x])

cdef inline num_t add(num_t a, num_t b) nogil:
    return a + b

cdef inline num_t sub(num_t a, num_t b) nogil:
    return a - b

cdef inline num_t mul(num_t a, num_t b) nogil:
    return a * b

@cython.cdivision(True)
cdef inline num_t div(num_t a, num_t b) nogil:
    cdef int check = <int> b
    if num_t is int:
        return a // b if check else int_inf
    else:
        return a / b if check else float_inf

@cython.nonecheck(False)
cdef inline void madd2d(num_t[:, :] a, num_t[:, :] b, num_t[:, :] to) nogil:
    mufunc2i1o2d[num_t](a, b, add[num_t], to)

@cython.nonecheck(False)
cdef inline void msub2d(num_t[:, :] a, num_t[:, :] b, num_t[:, :] to) nogil:
    mufunc2i1o2d[num_t](a, b, sub[num_t], to)

@cython.nonecheck(False)
cdef inline void mmul2d(num_t[:, :] a, num_t[:, :] b, num_t[:, :] to) nogil:
    mufunc2i1o2d[num_t](a, b, mul[num_t], to)

@cython.nonecheck(False)
cdef inline void mdiv2d(num_t[:, :] a, num_t[:, :] b, num_t[:, :] to) nogil:
    mufunc2i1o2d[num_t](a, b, div[num_t], to)


@cython.boundscheck(False)
@cython.wraparound(False)
@cython.nonecheck(False)
cdef inline num_t msum2d(num_t[:, :] n) nogil:
    cdef int h = <int>n.shape[0]
    cdef int w = <int>n.shape[1]

    cdef num_t result = 0

    cdef int x, y
    for y in prange(h):
        for x in prange(w):
            result += n[y, x]
    
    return result