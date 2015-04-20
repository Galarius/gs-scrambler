__author__ = 'galarius'

import math

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
    for i in range(0, size+1):
        m = []
        for j in range(0, size+1):
            idx = j + size*i
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
    for i in range(0, size-1):
        for j in range(0, size-1):
            msg_vec.append(M[i][j]) if not M[i][j] == 0 else None
    msg = vec_2_str(msg_vec)
    return msg