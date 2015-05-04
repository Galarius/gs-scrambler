# j.pyx
import numpy as np
cimport numpy as np

cdef extern from "jonson_c.h" namespace "jonson":
    cdef cppclass Jonson:
        Jonson(short int *data, int n) except +
        void calculate()
        int getSemiPeriod()

cdef class PyJonson:
    cdef Jonson *thisptr      # hold a C++ instance which we're wrapping
    def __cinit__(self, np.ndarray[np.int16_t, ndim=1] arr, N):
        self.thisptr = new Jonson(<short int *> arr.data, N)
    def __dealloc__(self):
        del self.thisptr
    def calculate(self):
        self.thisptr.calculate()
    def getSemiPeriod(self):
        return self.thisptr.getSemiPeriod()