__author__ = 'galarius'

from stego_helper import *
from q_matrix import *
from cpu_info import cpu_count


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
        msg_matr_encoded_bits = int_matrix_to_bits_matrix(msg_matr_encoded)
        msg_matr_encoded_array = np.array(msg_matr_encoded_bits)
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
            print '[Error] Wrong semi-period.'
            return message_part, chunck

        step = int(len(chunck) / float(semi_p))
        k = 0
        for i in range(semi_p, len(chunck), step):
            if k < len(message_part):
                bits = d_2_b(chunck[i], 16)
                sign = 1 if chunck[i] >= 0 else -1
                bits[0] = sign * message_part[k]
                chunck[i] = b_2_d(bits)
                k += 1
            else:
                break
        message_part = message_part[:k-1]
        return message_part, chunck

    def recover(self):
        pass