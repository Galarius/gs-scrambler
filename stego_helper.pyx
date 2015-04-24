# -*- coding: utf-8 -*-

__author__ = 'Ilya Shoshin'
__copyright__ = 'Copyright 2015, Ilya Shoshin'

import math

from math import floor
from numpy import sign
import struct
import numpy as np
from operator import itemgetter


def str_2_vec(str):
    """
    Convert string to vector of integers
    :param str: string
    :return:    [int, int, int, ...]
    """
    return [ord(i) for i in str]


def vec_2_str(vec):
    """
    Convert vector of integers to string.
    :param vec: [int, int, int, ...]
    :return:    string
    """
    return ''.join([str(unichr(i)) for i in vec])


def d_2_b(x, size=8):
    """
    Convert decimal to byte list
    :param x:    decimal
    :param size: the size of byte list
    :return: e.g. [0, 0, 1, ...]
    """
    s = sign(x)
    v = size * [None]
    for i in range(0, size):
        v[i] = abs(x) % 2
        x = int(floor(abs(x) / 2.0))
    return [s * x for x in v]


def b_2_d(x):
    """
    Convert byte list to decimal
    :param x:   byte list
    :return:    decimal
    """
    s = 0
    for i in range(0, len(x)):
        s += x[i] * 2 ** i
    return s


def message_to_matrix(message):
    """
    Transform message to matrix.
    To keep matrix square, it may be extended with zeros
    :param message: message to transform
    :return: matrix
    """
    msg_vec = str_2_vec(message)
    size = int(math.ceil(math.sqrt(len(msg_vec))))
    M = []
    for i in range(0, size + 1):
        m = []
        for j in range(0, size + 1):
            idx = j + size * i
            m.append(msg_vec[idx] if idx < len(msg_vec) else 0)
        M.append(m)
    return M


def matrix_to_message(M):
    """
    Transform matrix to message.
    :param M: matrix to transform
    :return: message
    """
    msg_vec = []
    size = int(len(M))
    for i in range(0, size - 1):
        for j in range(0, size - 1):
            msg_vec.append(M[i][j]) if not M[i][j] == 0 else None
    msg = vec_2_str(msg_vec)
    return msg


def int_matrix_to_bits_matrix(M):
    M_bits = []
    size = int(len(M))
    for i in range(0, size):
        m = []
        for j in range(0, size):
            m.append(d_2_b(M[i][j]))
        M_bits.append(m)
    return M_bits

def bits_matrix_to_int_matrix(M_bits):
    M = []
    size = int(len(M_bits))
    for i in range(0, size):
        m = []
        for j in range(0, size):
            m.append(b_2_d(M_bits[i][j]))
        M.append(m)
    return M

def audio_decode(in_data, frame_count, channels):
    """
    Convert a byte stream into tuple (left, right)
    """
    out = struct.unpack_from("%dh" % frame_count, in_data)
    # Convert 2 channels to numpy arrays
    if channels == 2:
        left = np.array(list(out[0::2]))
        right = np.array(list(out[1::2]))
    else:
        left = np.array(out)
        right = left
    return (left, right)


def audio_encode(samples, nchannels):
    """
    Convert a tuple (left, right) into byte stream
    """
    if nchannels == 2:
        data = [None] * (len(samples[0]) + len(samples[1]))
        data[::2] = samples[0]
        data[1::2] = samples[1]
    else:
        data = samples[0]
    frames = struct.pack("%dh" % len(data), *data)
    return frames


def jonson(data):
    """
    :param data:
    :return:
    The program process data with Jonson function to determine semi_period
    """
    # Process
    n = len(data)
    data_processed = []
    for tau in range(n):
        sum = 0
        for t in range(n - tau):
            sum += abs(float(data[t + tau]) - float(data[t]))
        if n - tau != 0:
            data_processed.append(1.0 / (n - tau) * sum)

    # calculate semi-period from calculated data
    l = int(0.1 * n)                 # 10 % from len
    del data_processed[-l:]          # remove last l elements
    del data_processed[:l]           # remove first l elements
    try:
        # find min
        semi_period_idx = min(enumerate(data_processed), key=itemgetter(1))[0]
    except ValueError:
        semi_period_idx = 0
    return semi_period_idx
