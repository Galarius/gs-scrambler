# -*- coding: utf-8 -*-

__author__ = 'Ilya Shoshin'
__copyright__ = 'Copyright 2015, Ilya Shoshin'

from q_matrix import *
import stego_helper as sh
import core
import numpy as np
import math
from extensions import elapsed_timer

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
            return chunk_source, chunk_container  # wrong semi-period, return original data
        # perform integration function from cpp code on current chunk with calculated unique step
        size = len(chunk_container)
        step = int(size / float(semi_p))
        container_processed, self.message_to_proc_part = core.integrate_c(chunk_container, size, semi_p, step, self.message_to_proc_part)
        if not len(self.message_to_proc_part):
            print colorize("Message integrated.", COLORS.OKBLUE)
        return chunk_source, container_processed

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

        size = len(chunk_container)
        container_processed, self.sync_mark_encoded_array = core.integrate_c(chunk_container, size, 0, 1, self.sync_mark_encoded_array)

        if not len(self.sync_mark_encoded_array):
            print colorize("Synchronization mark inserted.", COLORS.OKBLUE)
        else:
            print colorize("Not enough space to integrate sync marker.", COLORS.FAIL)

        self.synchronized = True
        return container_processed

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