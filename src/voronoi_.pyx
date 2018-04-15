import numpy as np
cimport numpy as np
import cython
import ctypes

from libcpp.string cimport string
from libcpp.vector cimport vector

cdef extern from "mwrap.h":
    vector[unsigned char] _thin(unsigned char* array, int m, int n, string imp_name)

@cython.boundscheck(False)
@cython.wraparound(False)
def thin(np.ndarray[np.uint8_t, ndim=2, mode="c"] nummat, string imp_name):
    r = nummat.shape[0]
    c = nummat.shape[1]
    data = _thin(&nummat[0,0], r, c, imp_name) 
    ndt = np.array(data, dtype=np.uint8).reshape((r,c))
    return ndt