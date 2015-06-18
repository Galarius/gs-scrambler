#!python
#cython: wraparound=False, boundscheck=False
__author__ = 'galarius'

import cython
import numpy as np
cimport numpy as np
from gsc_helper_template_py_wrapper cimport *
from cpython.string cimport PyString_AsString, PyString_FromStringAndSize, PyString_FromString

# Numpy must be initialized. When using numpy from C or Cython you must
# _always_ do that, or you will have segfaults
np.import_array()

cdef extern from "stdint.h" nogil:
    ctypedef   signed char  int8_t
    ctypedef   signed short int16_t

cdef extern from "gsc_core.h" namespace "gsc":
    cdef cppclass Core[I, B]:
        Core(const B * mark, size_t size, size_t frameSize, size_t scanBufferMaxSize, int security_or_capacity) except +
        size_t hide(const I * const seed, size_t s_size, I **container, size_t c_size, const B * const info, size_t i_size);
        size_t recover(const I * seed, size_t s_size, const I * container, size_t c_size, B **info);

cdef class PyCore:
    cdef Core[int16_t, int8_t] *thisptr      # hold a C++ instance which we're wrapping
    def __cinit__(self, np.ndarray[np.int8_t, ndim=1, mode="c"] mark not None, frame_size, scan_buffer_max_size, security_or_capacity):
        cdef size_t size = mark.size
        cdef int8_t *bptr = <int8_t*> &mark[0]
        self.thisptr = new Core[int16_t, int8_t](bptr, size, frame_size, scan_buffer_max_size, security_or_capacity)

    def __dealloc__(self):
        del self.thisptr

    def hide(self, np.ndarray[np.int16_t, ndim=1, mode="c"] seed not None,
                   np.ndarray[np.int16_t, ndim=1, mode="c"] container not None,
                   np.ndarray[np.int8_t, ndim=1, mode="c"] info not None):
        cdef int16_t *s_ptr = <int16_t*> &seed[0]
        cdef size_t s_size = seed.size
        cdef int16_t *c_ptr = <int16_t *> &container[0]
        cdef size_t c_size = container.size
        cdef int8_t *i_ptr = <int8_t*> &info[0]
        cdef size_t i_size = info.size
        cdef size_t l = self.thisptr.hide(s_ptr, s_size, &c_ptr, c_size, i_ptr, i_size)
        info = info[l:]
        return container, info

    def recover(self, np.ndarray[np.int16_t, ndim=1, mode="c"] seed not None,
                      np.ndarray[np.int16_t, ndim=1, mode="c"] container not None):
        cdef int16_t* s_ptr = <int16_t*> &seed[0]
        cdef size_t s_size = seed.size
        cdef int16_t* c_ptr = <int16_t*> &container[0]
        cdef size_t c_size = container.size
        cdef int8_t* i_ptr = NULL
        cdef size_t i_size = self.thisptr.recover(s_ptr, s_size, c_ptr, c_size, &i_ptr)
        cdef np.npy_intp shape[1]
        shape[0] = <np.npy_intp> i_size
        ndarray = np.PyArray_SimpleNewFromData(1, shape, np.NPY_INT8, <void *> i_ptr)
        np.PyArray_UpdateFlags(ndarray, ndarray.flags.num | np.NPY_OWNDATA)
        return ndarray

def str_to_vec(source):
    """
    Convert string to vector of integers
    :param source: string
    :return:    np.array([int16, int16, int16, ...], dtype=np.int16)
    """
    cdef int16_t *dest = NULL
    cdef size_t size = len(source)
    # python string to char *: http://stackoverflow.com/a/5061862/2422367
    cdef str retval = PyString_FromStringAndSize(PyString_AsString(source), <Py_ssize_t>size)
    cdef char *text = PyString_AsString(retval)
    # call C func
    str_to_short_arr(text, size, &dest)
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

def vec_to_str(np.ndarray[np.int16_t, ndim=1] source):
    """
    Convert vector of integers to string.
    :param source: np.array([int16, int16, int16, ...], dtype=np.int16)
    :return:       string
    """
    cdef char *dest = NULL
    cdef size_t size = len(source)
    # call C func
    short_arr_to_str(<int16_t*> &source[0], size, &dest)
    pystring = PyString_FromString(dest)
    delete_char_arr(&dest)
    return pystring

def integer_to_bin_arr(x):
    """
    Convert integer to byte list
    :param x:    short integer
    :return: e.g. np.array([0, 0, 1, ...], dtype=np.int8)
    """
    cdef int8_t *dest = NULL
    # call C func
    size = short_to_binary_arr(x, &dest)
    cdef np.npy_intp shape[1]
    shape[0] = <np.npy_intp> size
    ndarray = np.PyArray_SimpleNewFromData(1, shape, np.NPY_INT8, <void *> dest)
    np.PyArray_UpdateFlags(ndarray, ndarray.flags.num | np.NPY_OWNDATA)
    return ndarray

def bin_arr_to_integer(np.ndarray[np.int8_t, ndim=1] bits):
    """
    Convert byte array to integer
    :param x:   np.array([0, 0, 1, ...], dtype=np.int8)
    :return:    integer
    """
    cdef int16_t x
    binary_arr_to_short(<int8_t*> &bits[0], x)
    return x

def calculate_semi_period_with_processed_data_c(np.ndarray[np.int16_t, ndim=1, mode="c"] samples not None, n):
    """
    Calculate semi-period for discrete function using Alter-Johnson formula:
       a(tau) = 1/(n-tau) * sum(t=1,t<n-tau, |f(t+tau) - f(t)|),
    n - total number of samples,
    :param arr: values of discrete function
    :param n:   number of samples
    :return: processed values, semi_period or -1
        semi_period = argmin(a(tau)),
        semi_period_min <= semi_period <= semi_period_max
    """
    cdef float *out_ptr = NULL
    semi_p = calculate_semi_period_short(<int16_t *> samples.data, n, &out_ptr)
    cdef np.npy_intp shape[1]
    shape[0] = <np.npy_intp> n
    ndarray = np.PyArray_SimpleNewFromData(1, shape, np.NPY_FLOAT32, <void *> out_ptr)
    np.PyArray_UpdateFlags(ndarray, ndarray.flags.num | np.NPY_OWNDATA)
    return ndarray, semi_p
