# -*- coding: utf-8 -*-
#cython: boundscheck=False, wraparound=False, nonecheck=False

__author__ = 'Ilya Shoshin'
__copyright__ = 'Copyright 2015, Ilya Shoshin'

import math

import numpy as np
import collections
import pyaudio
import core


def message_to_matrix(message):
    """
    Transform message to matrix.
    To keep matrix square, it may be extended with zeros
    :param message: message to transform
    :return: matrix
    """
    msg_vec = core.str_2_vec(message)
    size = int(math.ceil(math.sqrt(len(msg_vec))))
    matr = np.zeros((size, size), dtype=np.int16)
    for i in range(size):
        for j in range(size):
            k = j + size * i
            if k < len(msg_vec):
                matr[i][j] = msg_vec[k]
            else:
                break
    return matr


def matrix_to_message(m):
    """
    Transform matrix to message.
    :param M: matrix to transform
    :return: message
    """
    msg_vec = m.ravel()
    msg = core.vec_2_str(msg_vec)
    return msg


def int_matrix_to_bits_matrix(m):
    size = int(len(m))
    m_bits = np.zeros((size, size), dtype=list)
    for i in range(0, size):
        for j in range(0, size):
            m_bits[i][j] = core.d_2_b(m[i][j])
    return m_bits

def bits_matrix_to_int_matrix(m_bits):
    size = int(len(m_bits))
    m = np.zeros((size, size), dtype=np.int16)
    for i in range(0, size):
        for j in range(0, size):
            m[i][j] = core.b_2_d(m_bits[i][j])
    return m

def py_audio_format_to_num_py(fmt):
    if fmt == pyaudio.paInt16:
        return np.int16
    elif fmt == pyaudio.paFloat32:
        return np.float32
    else:
        print 'Unsupported format!'
    return -1

# based on: https://github.com/mgeier/python-audio/blob/master/audio-files/utility.py
def pcm_2_float(sig, dtype='float32'):
    """
    Convert PCM signal to floating point with a range from -1 to 1.
    :param sig: Input array, must have (signed) integral type.
    :param dtype: Desired (floating point) data type.
    :return: Normalized floating point data.
    """
    sig = np.asarray(sig)
    if sig.dtype.kind != 'i':
        raise TypeError("'sig' must be an array of signed integers")
    dtype = np.dtype(dtype)
    if dtype.kind != 'f':
        raise TypeError("'dtype' must be floating point type")
    return sig.astype(dtype) / dtype.type(np.iinfo(sig.dtype).max)

def float_2_pcm(sig, dtype='int16'):
    """
    Convert floating point signal with a range from -1 to 1 to PCM.
    :param sig: Input array, must have floating point type.
    :param dtype: Desired (integer) data type. [optional]
    :return: integer data.
    """
    sig = np.asarray(sig)
    if sig.dtype.kind != 'f':
        raise TypeError("'sig' must be a float array")
    dtype = np.dtype(dtype)
    if dtype.kind != 'i':
        raise TypeError("'dtype' must be signed integer type")
    return (sig * np.iinfo(dtype).max).astype(dtype)

def audio_decode(in_data, channels):
    result = np.fromstring(in_data, dtype=np.float32)
    result = float_2_pcm(result, np.int16)
    chunk_length = len(result) / channels
    output = np.reshape(result, (chunk_length, channels))
    l, r = np.copy(output[:, 0]), np.copy(output[:, 1])
    return l, r

def audio_encode(samples):
    l = pcm_2_float(samples[0], np.float32)
    r = pcm_2_float(samples[1], np.float32)
    interleaved = np.array([l, r]).flatten('F')
    out_data = interleaved.astype(np.float32).tostring()
    return out_data


def contains(small, big):
    for i in xrange(len(big)-len(small)+1):
        for j in xrange(len(small)):
            if big[i+j] != small[j]:
                break
        else:   # for else
            return i, i + len(small)
    return -1, 0

def rms(data):
    count = len(data)
    sum_squares = 0.0
    for sample in data:
        n = sample * (1.0/32768)
        sum_squares += n*n
    return math.sqrt( sum_squares / count )

def compare(x, y):
    return collections.Counter(x) == collections.Counter(y)

