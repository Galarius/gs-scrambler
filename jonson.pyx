# -*- coding: utf-8 -*-
__author__ = 'Ilya Shoshin'
__copyright__ = 'Copyright 2015, Ilya Shoshin'

# cimport cython
# from cython.parallel import prange
# import numpy as np
# cimport numpy as np
# from libc.math cimport abs
#
# @cython.boundscheck(False)
# @cython.wraparound(False)
# cdef jonson(np.ndarray[int] data, np.ndarray[double] data_processed):
#     # Convenience variables and loop indices.
#     cdef np.npy_intp n = data.shape[0]
#     cdef np.npy_intp tau, t, k
#
#     cdef cython.parallel.reduction[int] s = 0
#
#     # Zero a vector before repopulating it.
#     for t in range(n):
#         data_processed[t] = 0
#     with nogil:
#         for tau in prange(n):
#             sum = 0
#             for t in range(n - tau):
#                 sum += abs(data[t + tau] - data[t])
#             if n - tau != 0:
#                 data_processed[k] = (1.0 / (n - tau) * sum)
#                 k += 1

from cython.parallel import prange

cdef int func(Py_ssize_t n):
    cdef Py_ssize_t i

    for i in prange(n, nogil=True):
        if i == 8:
            with gil:
                raise Exception()
        elif i == 4:
            break
        elif i == 2:
            return i
