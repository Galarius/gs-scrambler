__author__ = 'galarius'

import stego_helper as sh

from q_matrix import *
import numpy as np
import math

class StegoMode:
    Encode = 0,
    Decode = 1


class StegoCore:
    def __init__(self, message, key, stego_mode):
        self.message = message
        self.key = key
        self.stego_mode = stego_mode
        self.message_to_proc_part = []
        self.prepare_message()

    def prepare_message(self):
        msg_matr = sh.message_to_matrix(self.message)
        msg_matr_encoded = QMatrix.encode_matrix_message(msg_matr, 1)
        msg_matr_encoded_bits = sh.int_matrix_to_bits_matrix(msg_matr_encoded)
        msg_matr_encoded_array = np.array(msg_matr_encoded_bits)
        self.message_to_proc_part = msg_matr_encoded_array.ravel()

    def recover_message(self):
        s = math.sqrt(len(self.message_to_proc_part)/8)
        msg_matr_encoded_array = np.reshape(self.message_to_proc_part, (s, s, 8))
        msg_matr_encoded_bits = msg_matr_encoded_array.tolist()
        msg_matr_encoded = sh.bits_matrix_to_int_matrix(msg_matr_encoded_bits)
        msg_matr = QMatrix.decode_matrix_message(msg_matr_encoded, 1)
        msg = sh.matrix_to_message(msg_matr)
        return msg

    def process(self, chunk):
        if self.stego_mode == StegoMode.Encode:
            if len(self.message_to_proc_part) > 0:
                self.message_to_proc_part, processed_chunk = self.hide(self.message_to_proc_part, chunk)
                return processed_chunk
            else:
                return chunk
        else:
            np.append(self.message_to_proc_part, self.recover(chunk))
            return chunk

    def hide(self, message_part, chunk):

        semi_p = sh.jonson(chunk)

        if semi_p == 0:
            print '[Error] Wrong semi-period.'
            return message_part, chunk

        length = len(chunk)
        step = int(length / float(semi_p))
        k = 0
        for i in range(semi_p, length, step):
            if k < len(message_part):
                bits = sh.d_2_b(chunk[i], 16)
                sign = 1 if chunk[i] >= 0 else -1
                bits[0] = sign * message_part[k]
                chunk[i] = sh.b_2_d(bits)
                k += 1
            else:
                break
        message_part = message_part[:k-1]
        return message_part, chunk

    def recover(self, chunk):

        semi_p = sh.jonson(chunk)

        message_part = []
        if semi_p == 0:
            print '[Error] Wrong semi-period.'
            return message_part

        length = len(chunk)
        step = int(length / float(semi_p))

        for i in range(semi_p, length, step):
            bits = sh.d_2_b(chunk[i], 16)
            message_part.append(abs(bits[0]))

        return message_part