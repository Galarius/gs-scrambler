# -*- coding: utf-8 -*-

__author__ = 'Ilya Shoshin'
__copyright__ = 'Copyright 2015, Ilya Shoshin'

from q_matrix import *
import stego_helper as sh
import core
import numpy as np
import math

from extensions import colorize, COLORS

FORMAT = np.int16

class StegoMode:
    """
    Enum to indicate process mode
    """
    Hide = 0
    Recover = 1


class StegoCore:
    """
    Perform hide/recover on the txt msg
    """
    SKIP_FRAMES_KEY = 'skip_frames'     # number of frames to skip before integration
    SYNC_MARK_KEY = 'sync_mark_key'     # synchronization mark

    BITS = 16
    SYNC_KEY = 8

    def __init__(self, stego_mode, **kwargs):
        """
        Init core class instance.

        :param stego_mode:  StegoMode.Hide or StegoMode.Recover
        :param kwargs:
                            1) StegoMode.Hide    - necessary keys:
                                a) StegoCore.SKIP_FRAMES_KEY - number of frames to skip before integration  [optional]
                                b) StegoCore.SYNC_MARK_KEY - text marker to perform synchronization         [optional]
                            2) StegoMode.Recover - optional key:
                                a) StegoCore.SKIP_FRAMES_KEY - number of frames to skip before integration  [optional]
                                b) StegoCore.SYNC_MARK_KEY - text marker to perform synchronization         [optional]
        """

        self.stego_mode = stego_mode    # encode or decode
        self.message_to_proc_part = []  # the part of message that left to be integrated
        self.skip_frames = 0            # how many frames shoud be skipped
        self.mediate_length = 0         # length of message to recover (length of bits array)
        # synchronization
        self.sync_mark = ''                 # synchronization mark
        self.sync_mark_encoded_array = []   # synchronization mark encoded
        self.sync_mark_temp_encoded_array = []   # list that may contain sync mark
        self.synchronized = False           # is synchronization performed
        if stego_mode == StegoMode.Hide:
            if StegoCore.SKIP_FRAMES_KEY in kwargs:
                # set number of frames to skip
                self.skip_frames = kwargs[StegoCore.SKIP_FRAMES_KEY]
            if StegoCore.SYNC_MARK_KEY in kwargs:
                self.sync_mark = kwargs[StegoCore.SYNC_MARK_KEY]
                self.__synchronization_prepare()
            else:
                self.synchronized = True
        else:
            if StegoCore.SKIP_FRAMES_KEY in kwargs:
                # set number of frames to skip
                self.skip_frames = kwargs[StegoCore.SKIP_FRAMES_KEY]
            if StegoCore.SYNC_MARK_KEY in kwargs:
                self.sync_mark = kwargs[StegoCore.SYNC_MARK_KEY]
                self.__synchronization_prepare()
            else:
                self.synchronized = True

    def hide(self, message, key):
        """
        Enable hiding process.
        :param message: message to hide
        :param key:     key to encode message with
        :return:        additional key to extract message
        """
        # Encode msg and make some preprocessing staff, get msg bits array
        self.synchronized = False
        return self.__prepare_message(message, key)

    def recover(self, session_key, user_key):
        """
        Recover message by extracting it with session_key and decoding it with user_key.
        :param user_key:
        :param session_key:
        :return: recovered message
        """
        return self.__recover_message(session_key, user_key)

    def process(self, chunk_source, chunk_container):
        """
        Depending on the init args this method will perform integration or recovering.
        :param chunk: chunk to be used as container to perform integration or recovering
        :return: processed chunk or the original chunk
        """

        # rms = self.rms(chunk_container.tolist())
        # if rms > 0:
        #     decibel = 20 * math.log10(rms)
        #     print decibel

        #if not self.skip_frames:                                           # if no frames left to skip
        if self.stego_mode == StegoMode.Hide:                               # if hiding
            # hide
            if self.synchronized:
                if len(self.message_to_proc_part) > 0:                          # if there is smth to hide
                    return self.__hide(chunk_source, chunk_container)           # hide msg and return processed chunk
            else:
                return self.__synchronization_put(chunk_source), chunk_container
        else:
            # recover
            if self.synchronized:
                self.__recover(chunk_source, chunk_container)                   # recover msg part from a chunk
            else:
                self.__synchronization_detect(chunk_source)
        #else:
        #    self.skip_frames -= 1

        return chunk_source, chunk_container                                         # return original chunk

    def __recover_message(self, mediate_length, key):
        """
        After extraction is completed, call this method to decode and extract original message.
        :param mediate_length: the mediate length of hidden message (necessary to perform decoding)
        :return: extracted message
        """

        if not mediate_length:
            mediate_length = mediate_length
            if mediate_length <= 0:
                raise AttributeError('Necessary argument not specified!')

        if self.message_to_proc_part > mediate_length:
            self.message_to_proc_part = self.message_to_proc_part[:mediate_length]
        elif self.message_to_proc_part < mediate_length:
            raise RuntimeError("Couldn't extract message with provided argument.")

        s = int(math.ceil(math.sqrt(len(self.message_to_proc_part) / StegoCore.BITS)))
        try:
            msg_matrix_encoded_array = np.reshape(self.message_to_proc_part, (s, s, StegoCore.BITS))
        except ValueError:
            print colorize("Wrong session key. Can't extract message.", COLORS.FAIL)
            return ''
        msg_matrix_encoded_bits = msg_matrix_encoded_array.tolist()
        msg_matrix_encoded = sh.bits_matrix_to_int_matrix(msg_matrix_encoded_bits)
        msg_matrix = QMatrix.decode_matrix_message(msg_matrix_encoded, key)
        msg = sh.matrix_to_message(msg_matrix)
        return msg

    def __prepare_message(self, message, key):
        """
        Prepare message to be integrated, by encoding and several additional transforms
        :return: the mediate length of hidden message (necessary to perform decoding later)
        """
        # 1) Transform to matrix
        msg_matrix = sh.message_to_matrix(message)
        # 2) Encode with q-matrix
        msg_matrix_encoded = QMatrix.encode_matrix_message(msg_matrix, key)
        # 3) Convert to bits
        msg_matrix_encoded_bits = sh.int_matrix_to_bits_matrix(msg_matrix_encoded)
        # 4) Convert to numpy nested arrays
        msg_matrix_encoded_array = np.array(msg_matrix_encoded_bits, dtype=FORMAT)
        # 5) Convert 2D array to 1D array
        self.message_to_proc_part = msg_matrix_encoded_array.ravel()
        # 6) Save array length to recover message later
        return len(self.message_to_proc_part)

    def __hide(self, chunk_source, chunk_container):
        """
        Hide message part in container
        :param chunk: chunk to be used as container
        :return:      processed or the original chunk
        """
        # calculate semi-period
        semi_p = core.calculate_semi_period_c(chunk_source, len(chunk_source))

        if semi_p == 0:
            return chunk_source, chunk_container            # wrong semi-period, return original data

        # perform lsb method on current chunk with calculated unique step
        temp = [x for x in chunk_container]
        length = len(chunk_container)
        step = int(length / float(semi_p))
        for i in range(semi_p, length, step):
            if len(self.message_to_proc_part) > 0:
                bits = core.d_2_b(chunk_container[i])
                sign = 1 if chunk_container[i] >= 0 else -1
                bits[0] = sign * self.message_to_proc_part[0]
                self.message_to_proc_part = self.message_to_proc_part[1:]
                temp[i] = core.b_2_d(bits)
            else:
                break

        # chunk_container = np.array([-8 for x in range(len(chunk_container))], dtype=FORMAT)
        # return chunk_source, chunk_container

        if not len(self.message_to_proc_part):
            print colorize("Message integrated.", COLORS.OKBLUE)

        return chunk_source, np.asarray(temp, dtype=np.int16)

    def __recover(self, chunk_source, chunk_container):
        """
        Recover message part from container
        :param chunk: container
        """

        # calculate semi-period
        semi_p = core.calculate_semi_period_c(chunk_source, len(chunk_source))

        if semi_p != 0:

            message_part = []
            length = len(chunk_container)
            step = int(length / float(semi_p))
            for i in range(semi_p, length, step):
                bits = core.d_2_b(chunk_container[i])
                message_part.append(abs(bits[0]))

            # extend msg part array
            self.message_to_proc_part.extend(message_part)

    #--------------------------------------------------------------
    # Synchronization
    #--------------------------------------------------------------

    def __synchronization_prepare(self):
        """
        Prepare sync text marker to be integrated, by encoding and several additional transforms
        """
        # 1) Transform to matrix
        sync_marker_matrix = sh.message_to_matrix(self.sync_mark)
        # 2) Encode with q-matrix
        sync_marker_matrix_encoded = QMatrix.encode_matrix_message(sync_marker_matrix, StegoCore.SYNC_KEY)
        # 3) Convert to bits
        sync_marker_matrix_encoded_bits = sh.int_matrix_to_bits_matrix(sync_marker_matrix_encoded)
        # 4) Convert to numpy nested arrays
        sync_marker_matrix_encoded_array = np.array(sync_marker_matrix_encoded_bits, dtype=FORMAT)
        # 5) Convert 2D array to 1D array
        self.sync_mark_encoded_array = sync_marker_matrix_encoded_array.ravel()

    def __synchronization_put(self, chunk_container):
        import time
        if self.sync_mark == '' or self.synchronized:
            print colorize("No sync marker.", COLORS.FAIL)
            return chunk_container

        temp = [x for x in chunk_container]
        for i in range(len(chunk_container)):
            if len(self.sync_mark_encoded_array) > 0:
                bits = core.d_2_b(chunk_container[i])
                sign = 1 if chunk_container[i] >= 0 else -1
                bits[0] = sign * self.sync_mark_encoded_array[0]
                self.sync_mark_encoded_array = self.sync_mark_encoded_array[1:]
                temp[i] = core.b_2_d(bits)
            else:
                break

        print chunk_container.tolist()
        # chunk_container = np.array([69 for x in range(len(chunk_container))], dtype=np.int16)
        # time.sleep(3)
        # chunk_container = np.array([188, 183, 180, 169, 162, 166, 171, 159, 147, 141, 136, 147, 146, 144, 140, 124, 129, 133, 146, 149, 141, 133, 119, 120, 117, 119, 117, 107, 114, 114, 119, 114, 109, 108, 96, 94, 92, 93, 86, 87, 96, 95, 100, 92, 89, 85, 73, 81, 81, 77, 83, 82, 79, 76, 71, 66, 67, 73, 74, 72, 72, 62, 52, 57, 63, 59, 54, 44, 37, 42, 44, 52, 53, 47, 38, 37, 46, 41, 42, 42, 38, 44, 41, 40, 39, 39, 37, 35, 33, 33, 42, 40, 41, 38, 28, 29, 28, 34, 40, 43, 36, 27, 37, 36, 38, 35, 22, 21, 26, 36, 40, 44, 36, 31, 39, 37, 45, 47, 46, 48, 36, 37, 39, 36, 36, 33, 28, 20, 21, 18, 18, 25, 22, 24, 22, 9, 1, -1, 0, 3, 0, -7, -14, -15, -11, -18, -18, -21, -23, -18, -23, -29, -31, -32, -26, -17, -18, -16, -16, -24, -16, -10, -11, -3, 4, 5, 4, 9, 14, 18, 26, 28, 36, 43, 45, 55, 56, 65, 78, 83, 93, 96, 100, 108, 122, 135, 139, 140, 138, 151, 163, 171, 182, 179, 187, 205, 221, 226, 220, 216, 216, 225, 229, 225, 225, 227, 240, 256, 246, 235, 241, 258, 271, 260, 239, 225, 229, 240, 242, 244, 239, 233, 234, 239, 234, 221, 219, 218, 215, 208, 207, 205, 199, 199, 198, 192, 183, 177, 171, 162, 163, 167, 164, 158, 154, 145, 134, 125, 120, 118, 119, 122, 116, 103, 100, 102, 99, 103, 106, 102, 103, 94, 82, 79, 78, 88, 91, 80, 76, 82, 106, 119, 103, 81, 90, 128, 144, 121, 81, 70, 97, 118, 116, 92, 78, 94, 113, 109, 79, 58, 66, 89, 102, 90, 60, 35, 33, 40, 48, 42, 23, 5, 0, 3, 11, 12, 0, -3, -10, -21, -25, -30, -32, -34, -34, -32, -35, -45, -57, -59, -45, -44, -55, -57, -68, -75, -73, -83, -94, -99, -100, -106, -109, -103, -105, -112, -115, -113, -109, -113, -123, -131, -138, -139, -137, -136, -132, -129, -125, -123, -129, -137, -133, -129, -124, -119, -125, -122, -127, -128, -117, -120, -129, -139, -150, -154, -153, -154, -160, -167, -181, -190, -197, -212, -231, -256, -279, -294, -303, -310, -327, -343, -355, -359, -366, -384, -405, -422, -432, -451, -475, -496, -515, -519, -525, -542, -558, -583, -585, -572, -588, -611, -629, -641, -646, -661, -675, -675, -674, -673, -678, -694, -699, -697, -691, -681, -684, -690, -694, -699, -683, -666, -656, -642, -642, -643, -635, -629, -621, -605, -582, -561, -545, -537, -534, -528, -512, -493, -473, -448, -433, -430, -416, -397, -375, -345, -328, -320, -304, -288, -272, -257, -247, -231, -213, -201, -183, -164, -139, -121, -117, -110, -93, -62, -42, -32, -30, -36, -29, -14, 6, 21, 32, 36, 43, 59, 79, 104, 131, 154, 152, 137, 135, 156, 192, 219, 234, 239, 244, 267, 289, 303, 313, 319, 341, 361, 368, 371, 380, 407, 425, 433, 441, 456, 482, 498, 501, 504, 524, 556, 578, 584, 578, 586, 604, 617, 626, 632, 647, 661, 668, 669, 678, 700, 720, 729, 722, 719, 723, 737, 743, 733, 737, 745, 749, 751, 734, 724, 734, 748, 759, 753, 746, 740, 741, 747, 739, 727, 714, 710, 709, 698, 682, 666, 659, 652, 649, 645, 634, 624, 612, 602, 601, 599, 589, 577, 563, 550, 538, 531, 521, 499, 487, 482, 481, 465, 447, 438, 423, 419, 403, 382, 367, 358, 358, 344, 328, 309, 287, 276, 262, 250, 241, 228, 215, 204, 191, 179, 169, 160, 147, 125, 108, 91, 74, 62, 42, 30, 21, 0, -22, -44, -58, -58, -63, -80, -104, -126, -146, -159, -174, -195, -203, -208, -219, -239, -262, -274, -280, -281, -292, -317, -337, -349, -350, -347, -350, -357, -376, -388, -388, -388, -393, -395, -390, -391, -399, -411, -419, -416, -413, -409, -404, -408, -406, -398, -391, -385, -386, -391, -394, -382, -370, -366, -364, -370, -371, -365, -354, -337, -332, -348, -361, -355, -343, -335, -335, -339, -349, -355, -355, -355, -346, -337, -330, -317, -310, -310, -318, -325, -313, -301, -296, -301, -324, -343, -346, -338, -333, -329, -336, -341, -326, -320, -335, -362, -373, -373, -370, -371, -374, -367, -354, -348, -350, -364, -381, -380, -378, -392, -412, -424, -421, -407, -394, -400, -420, -437, -432, -416, -410, -412, -412, -401, -393, -393, -396, -392, -373, -354, -348, -350, -344, -328, -308, -296, -299, -296, -281, -262, -243, -235, -236, -231, -223, -215, -202, -199, -195, -184, -173, -155, -138, -128, -115, -106, -102, -102, -103, -98, -88, -80, -89, -96, -83, -57, -35, -24, -24, -33, -33, -20, -12, -16, -23, -20, 0, 11, 14, 18, 18, 32, 49, 52, 55, 54, 53, 58, 55, 45, 36, 42, 58, 67, 71, 74, 76, 82, 94, 97, 94, 92, 93, 108, 137, 154, 140, 115, 106, 124, 152, 166, 172, 176, 184, 187, 185, 190, 197, 209, 225, 231, 227, 221, 228, 244, 262, 272, 268, 267, 267, 281, 294, 291, 294, 306, 314, 314, 316, 317, 320, 341, 356, 359, 362, 357, 360, 370, 376, 380, 364, 345, 345, 356, 354, 344, 337, 334, 344, 349, 338, 329, 333, 348, 352, 336, 319, 310, 309, 319, 329, 314, 299, 312, 327, 334, 322, 305, 299, 296, 306, 311, 287, 274, 293, 318, 318, 292, 265, 257, 268, 297, 311, 295, 277, 273, 286, 297, 293, 280, 258, 246, 246, 253, 269, 284, 290, 291, 279, 246, 220, 219, 241, 261, 271, 259, 237, 230, 224, 224, 223, 215, 210, 205, 205, 190, 155, 147, 163, 184, 185, 154, 130, 108, 105, 120, 125, 133, 120, 99, 85, 72, 78, 87, 96, 94, 71, 44, 26, 18, 23, 40, 49, 45, 30, 13, 4, 6, 8, 3, -2, -7, -11, -20, -23, -21, -22, -15, -16, -30, -38, -46, -44, -35, -32, -37, -52, -60, -61, -60, -58, -56, -62, -64, -53, -49, -46, -37, -37, -41, -42, -43, -42, -37, -39, -44, -42, -42, -41, -40, -34, -28, -44, -55, -67, -77, -78, -89, -99, -104, -95, -85, -91, -100], dtype=FORMAT)
        # return chunk_container


        if not len(self.sync_mark_encoded_array):
            print colorize("Synchronization mark inserted.", COLORS.OKBLUE)
        else:
            print colorize("Not enough space to integrate sync marker.", COLORS.FAIL)

        self.synchronized = True
        return np.asarray(temp, dtype=np.int16)

    def __synchronization_detect(self, chunk_container):

        if self.sync_mark == '':
            print "Mark is empty"
            return False

        if len(self.sync_mark_temp_encoded_array) >= 3 * len(chunk_container):
            self.sync_mark_temp_encoded_array = self.sync_mark_temp_encoded_array[len(chunk_container):]

        for i in range(len(chunk_container)):
            bits = core.d_2_b(chunk_container[i])
            self.sync_mark_temp_encoded_array.append(abs(bits[0]))

        pos, length = sh.contains(self.sync_mark_encoded_array.tolist(), self.sync_mark_temp_encoded_array)
        self.synchronized = pos >= 0 and length > 0

        if self.synchronized:
            print colorize("Synchronization completed.", COLORS.OKGREEN)
            del self.sync_mark_temp_encoded_array[:]
        return self.synchronized