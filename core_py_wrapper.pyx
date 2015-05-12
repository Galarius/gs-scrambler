#!python
#cython: wraparound=False, boundscheck=False
__author__ = 'galarius'

import cython
import numpy as np
cimport numpy as np

# Numpy must be initialized. When using numpy from C or Cython you must
# _always_ do that, or you will have segfaults
np.import_array()

#--------------------------------------------------------------------
# non-template core methods
#--------------------------------------------------------------------

cdef extern from "core.h":
    int calculate_semi_period(short int* data, int n);
    int integrate(short int **container, int size, int begin, int step, short int *info);
    int deintegrate(const short int *container, int size, int begin, int step, short int **info);


def calculate_semi_period_c(np.ndarray[np.int16_t, ndim=1, mode="c"] samples not None, n):
    """
    Calculate semi-period for discrete function using Alter-Johnson formula:
       a(tau) = 1/(n-tau) * sum(t=1,t<n-tau, |f(t+tau) - f(t)|),
    n - total number of samples,
    :param arr: values of discrete function
    :param n:   number of samples
    :return: semi_period
        semi_period = argmin(a(tau)),
        semi_period_min <= semi_period <= semi_period_max
    """
    return calculate_semi_period(<short int *> samples.data, n)

def integrate_c(np.ndarray[np.int16_t, ndim=1, mode="c"] container not None, n, begin, step, np.ndarray[np.int16_t, ndim=1, mode="c"] info not None):
    cdef short int *ptr = <short int *> container.data
    cdef l = integrate(&ptr, n, begin, step, <short int *> info.data)
    info = info[l:]
    return container, info


#--------------------------------------------------------------------
# template core methods
#--------------------------------------------------------------------

from core_py_wrapper_template cimport *
from cpython.string cimport PyString_AsString, PyString_FromStringAndSize, PyString_FromString

def str_2_vec(source):
    """
    Convert string to vector of integers
    :param source: string
    :return:    np.array([int16, int16, int16, ...], dtype=np.int16)
    """
    cdef short int *dest = NULL
    cdef int size = len(source)
    # python string to char *: http://stackoverflow.com/a/5061862/2422367
    cdef str retval = PyString_FromStringAndSize(PyString_AsString(source), <Py_ssize_t>size)
    cdef char *text = PyString_AsString(retval)
    # call C func
    str2vec_short(text, &dest, size)
    # Create a C array to describe the shape of the ndarray
    cdef np.npy_intp shape[1]
    shape[0] = <np.npy_intp> size
    # Use the PyArray_SimpleNewFromData function from numpy to create a
    # new Python object pointing to the existing data (without copying)
    # http://docs.scipy.org/doc/numpy/user/c-info.how-to-extend.html
    ndarray = np.PyArray_SimpleNewFromData(1, shape, np.NPY_INT16, <void *> dest)
    # Tell Python that it can deallocate the memory when the ndarray
    # object gets garbage collected
    # As the OWNDATA flag of an array is read-only in Python, we need to
    # call the C function PyArray_UpdateFlags
    np.PyArray_UpdateFlags(ndarray, ndarray.flags.num | np.NPY_OWNDATA)
    return ndarray

def vec_2_str(np.ndarray[np.int16_t, ndim=1] source):
    """
    Convert vector of integers to string.
    :param source: np.array([int16, int16, int16, ...], dtype=np.int16)
    :return:       string
    """
    cdef char *dest = NULL
    cdef int size = len(source)
    # call C func
    vec2str_short(<short int *> source.data, &dest, size)
    pystring = PyString_FromString(dest);
    delete_char_arr(&dest)
    return pystring

def d_2_b(x):
    """
    Convert integer to byte list
    :param x:    short integer
    :return: e.g. np.array([0, 0, 1, ...], dtype=np.int16)
    """
    cdef short int *dest = NULL
    # call C func
    size = d2b_short(x, &dest)
    cdef np.npy_intp shape[1]
    shape[0] = <np.npy_intp> size
    ndarray = np.PyArray_SimpleNewFromData(1, shape, np.NPY_INT16, <void *> dest)
    np.PyArray_UpdateFlags(ndarray, ndarray.flags.num | np.NPY_OWNDATA)
    return ndarray


def b_2_d(np.ndarray[np.int16_t, ndim=1] bits):
    """
    Convert byte array to integer
    :param x:   np.array([0, 0, 1, ...], dtype=np.int16)
    :return:    integer
    """
    cdef short int x
    b2d_short(<short int *> bits.data, x)
    return x
