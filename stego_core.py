# -*- coding: utf-8 -*-

__author__ = 'Ilya Shoshin'
__copyright__ = 'Copyright 2015, Ilya Shoshin'

from q_matrix import *
import stego_helper as sh
import numpy as np
import math

class StegoMode:
    """
    Enum to indicate process mode
    """
    Hide = 0,
    Recover = 1


class StegoCore:
    """
    Perform hide/recover on the txt msg
    """
    def __init__(self, message, key, stego_mode):
        self.key = key                  # key to encrypt/decrypt
        self.stego_mode = stego_mode    # encode or decode
        self.message_to_proc_part = []  # the part of message that left to be integrated
        # Encode msg and make some preprocessing staff
        if stego_mode == StegoMode.Hide:
            self.message = message      # msg to hide
            self.__prepare_message()

    def process(self, chunk_source, chunk_container):
        """
        Depending on the init args this method will perform integration or recovering.
        :param chunk: chunk to be used as container to perform integration or recovering
        :return: processed chunk or the original chunk
        """
        if self.stego_mode == StegoMode.Hide:     # if hiding
            if len(self.message_to_proc_part) > 0:  # if there is smth to hide
                return self.__hide(chunk_source, chunk_container)       # hide msg and return processed chunk
        else:
            self.__recover(chunk_source, chunk_container)               # recover msg part from a chunk

        return chunk_container                                          # return original chunk

    def recover_message(self):
        """
        After extraction is completed, call this method to decode and extract original message.
        """
        print self.message_to_proc_part
        bits = 8
        s = int(math.sqrt(len(self.message_to_proc_part) / bits))
        if len(self.message_to_proc_part) > s * s * bits:
            l = s * s * bits
            self.message_to_proc_part = self.message_to_proc_part[:l]
        msg_matrix_encoded_array = np.reshape(self.message_to_proc_part, (s, s, bits))
        msg_matrix_encoded_bits = msg_matrix_encoded_array.tolist()
        msg_matrix_encoded = sh.bits_matrix_to_int_matrix(msg_matrix_encoded_bits)
        msg_matrix = QMatrix.decode_matrix_message(msg_matrix_encoded, self.key)
        msg = sh.matrix_to_message(msg_matrix)
        return msg

    def __prepare_message(self):
        """
        Prepare message to be integrated, by encoding and several additional transforms
        """
        # 1) Transform to matrix
        msg_matrix = sh.message_to_matrix(self.message)
        # 2) Encode with q-matrix
        msg_matrix_encoded = QMatrix.encode_matrix_message(msg_matrix, self.key)
        # 3) Convert to bits
        msg_matrix_encoded_bits = sh.int_matrix_to_bits_matrix(msg_matrix_encoded)
        # 4) Convert to numpy nested arrays
        msg_matrix_encoded_array = np.array(msg_matrix_encoded_bits)
        # 5) Convert 2D array to 1D array
        self.message_to_proc_part = msg_matrix_encoded_array.ravel()

        print self.message_to_proc_part.tolist()

    def __hide(self, chunk_source, chunk_container):
        """
        Hide message part in container
        :param chunk: chunk to be used as container
        :return:      processed or the original chunk
        """

        semi_p = sh.jonson(chunk_source)   # calculate semi-period
        if semi_p == 0:
            return chunk_container            # wrong semi-period, return original data

        # perform lsb method on current chunk with calculated unique step
        length = len(chunk_container)
        step = int(length / float(semi_p))
        k = 0
        for i in range(semi_p, length, step):
            if k < len(self.message_to_proc_part):
                bits = sh.d_2_b(chunk_container[i], 16)
                sign = 1 if chunk_container[i] >= 0 else -1
                bits[0] = sign * self.message_to_proc_part[k]
                chunk_container[i] = sh.b_2_d(bits)
                print semi_p, chunk_container[i]
                k += 1
            else:
                break

        self.message_to_proc_part = self.message_to_proc_part[:k-1]
        return chunk_container

    def __recover(self, chunk_source, chunk_container):
        """
        Recover message part from container
        :param chunk: container
        """
        semi_p = sh.jonson(chunk_source)   # calculate semi-period
        message_part = []
        if semi_p != 0:

            length = len(chunk_container)
            step = int(length / float(semi_p))

            for i in range(semi_p, length, step):
                print semi_p, chunk_container[i]
                bits = sh.d_2_b(chunk_container[i], 16)
                message_part.append(abs(bits[0]))

            # extend msg part array
            self.message_to_proc_part.extend(message_part)