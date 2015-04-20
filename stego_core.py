__author__ = 'galarius'

from stego_helper import *
from q_matrix import *

def integrate(msg, chunck):

    semi_p = jonson(chunck)

    if semi_p == 0:
        return chunck

    M = message_to_matrix(msg)
    E = QMatrix.encode_matrix_message(M, 1)

    step = len(chunck) / semi_p
    Enp = np.array(E)
    El = Enp.ravel()
    k = 0
    for i in range(0, len(chunck), step):
        if k < len(El):
            chunck[i] = El[k]
            k = k + 1
        else:
            break
    return chunck