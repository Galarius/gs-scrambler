__author__ = 'galarius'

import numpy as np
cimport numpy as np

cdef extern from "core.h":
    int calculate_semi_period(short int* data, int n);

def calculate_semi_period_c(np.ndarray[np.int16_t, ndim=1] arr, n):
    return calculate_semi_period(<short int *> arr.data, n)