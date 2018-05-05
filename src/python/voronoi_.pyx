import numpy as np
cimport numpy as np
import cython

from libcpp.string cimport string

cdef extern from "<valarray>" namespace "std":
    cdef cppclass valarray[T]:
        valarray()
        valarray(unsigned char)
        T& operator[](unsigned char)

cdef extern from "mwrap.h":
    void _thinImplicit(unsigned char* array, int m, int n, string imp_name)
    valarray[unsigned char] _thinExplicit(unsigned char* array, int m, int n, string imp_name)

# returns None, modifies source array
@cython.boundscheck(False)
@cython.wraparound(False)
cpdef void thinImplicit(np.ndarray[np.uint8_t, ndim=2, mode="c"] nummat, string imp_name):
    cdef int r = nummat.shape[0]
    cdef int c = nummat.shape[1]
    _thinImplicit(&nummat[0,0], r, c, imp_name) 

# returns numpy array
@cython.boundscheck(False)
@cython.wraparound(False)
cpdef np.ndarray[np.uint8_t, ndim=2, mode="c"] thinExplicit(np.ndarray[np.uint8_t, ndim=2, mode="c"] nummat, string imp_name):
    cdef int r = nummat.shape[0]
    cdef int c = nummat.shape[1]
    cdef int i = 0
    cdef int msize = r*c;
    cdef valarray[unsigned char] v = _thinExplicit(&nummat[0,0], r, c, imp_name)
    cdef unsigned char[:] ndt =  np.empty(msize, dtype=np.uint8)

    for i in range(msize):
        ndt[i] = v[i]
    
    return np.asarray(ndt).reshape((r,c))