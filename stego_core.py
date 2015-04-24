__author__ = 'galarius'

from stego_helper import *
from q_matrix import *


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
        msg_matr = message_to_matrix(self.message)
        msg_matr_encoded = QMatrix.encode_matrix_message(msg_matr, 1)
        msg_matr_encoded_array = np.array(msg_matr_encoded)
        self.message_to_proc_part = msg_matr_encoded_array.ravel()

    def process(self, chunk):
        processed_chunk = []
        if self.stego_mode == StegoMode.Encode:
            if len(self.message_to_proc_part) > 0:
                self.message_to_proc_part, processed_chunk = self.hide(self.message_to_proc_part, chunk)
            else:
                processed_chunk = chunk
        else:
            pass
        return processed_chunk


    def hide(self, message_part, chunck):
        semi_p = jonson(chunck)
        if semi_p == 0:
            print 'Wrong semi period'
            return message_part, chunck

        step = len(chunck) / semi_p
        k = 0
        for i in range(0, len(chunck), step):
            print i, k
            if k < len(message_part):
                chunck[i] = message_part[k]
                k = k + 1
            else:
                break
        message_part = message_part[:k-1]
        print len(message_part)
        return message_part, chunck

    def recover(self):
        pass