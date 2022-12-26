# -*- coding: utf-8 -*-

__author__ = "Ilya Shoshin"
__copyright__ = "Copyright 2015, Ilya Shoshin"

from .gs_q_matrix import *
from . import gs_helper as sh
from . import gsc_core
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

    SKIP_FRAMES_KEY = "skip_frames"  # number of frames to skip before integration
    SYNC_MARK_KEY = "sync_mark_key"  # synchronization mark
    SECURITY_OR_CAPACITY = "security_or_capacity"  # what is more important

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

        self.stego_mode = stego_mode  # encode or decode
        self.message_to_proc_part = np.empty(
            0, dtype=np.int8
        )  # the part of message that left to be integrated
        self.skip_frames = 0  # how many frames shoud be skipped
        self.mediate_length = 0  # length of message to recover (length of bits array)
        self.security_or_capacity = 0
        # synchronization
        self.sync_mark = ""  # synchronization mark
        self.sync_mark_encoded_array = np.empty(
            0, dtype=np.int8
        )  # synchronization mark encoded
        self.sync_mark_temp_encoded_array = []  # list that may contain sync mark
        if stego_mode == StegoMode.Hide:
            if StegoCore.SKIP_FRAMES_KEY in kwargs:
                # set number of frames to skip
                self.skip_frames = kwargs[StegoCore.SKIP_FRAMES_KEY]
        else:
            if StegoCore.SKIP_FRAMES_KEY in kwargs:
                # set number of frames to skip
                self.skip_frames = kwargs[StegoCore.SKIP_FRAMES_KEY]

        if StegoCore.SYNC_MARK_KEY in kwargs:
            self.sync_mark = kwargs[StegoCore.SYNC_MARK_KEY]
            self.__synchronization_prepare()

            if StegoCore.SECURITY_OR_CAPACITY in kwargs:
                self.security_or_capacity = kwargs[StegoCore.SECURITY_OR_CAPACITY]

            self.gsc = gsc_core.PyCore(
                self.sync_mark_encoded_array, 1024, 3 * 1024, self.security_or_capacity
            )
        else:
            print(colorize("Error: no sync mark provided", COLORS.FAIL))

    def hide(self, message, key):
        """
        Enable hiding process.
        :param message: message to hide
        :param key:     key to encode message with
        :return:        additional key to extract message
        """
        # Encode msg and make some preprocessing staff, get msg bits array
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
        if self.stego_mode == StegoMode.Hide:  # if hiding
            # hide
            if len(self.message_to_proc_part) > 0:  # if there is smth to hide
                return self.__hide(
                    chunk_source, chunk_container
                )  # hide msg and return processed chunk
        else:
            # recover
            self.__recover(
                chunk_source, chunk_container
            )  # recover msg part from a chunk

        return chunk_source, chunk_container  # return original chunk

    def __recover_message(self, mediate_length, key):
        """
        After extraction is completed, call this method to decode and extract original message.
        :param mediate_length: the mediate length of hidden message (necessary to perform decoding)
        :return: extracted message
        """

        if not mediate_length:
            mediate_length = mediate_length
            if mediate_length <= 0:
                raise AttributeError("Necessary argument not specified!")

        if len(self.message_to_proc_part) > mediate_length:
            self.message_to_proc_part = self.message_to_proc_part[:mediate_length]
        elif len(self.message_to_proc_part) < mediate_length:
            raise RuntimeError("Couldn't extract message with provided arguments.")

        s = int(math.ceil(math.sqrt(len(self.message_to_proc_part) / StegoCore.BITS)))
        try:
            msg_matrix_encoded_array = np.reshape(
                self.message_to_proc_part, (s, s, StegoCore.BITS)
            )
        except ValueError:
            print(colorize("Wrong session key. Can't extract message.", COLORS.FAIL))
            return ""

        msg_matrix_encoded = sh.bits_matrix_to_int_matrix(msg_matrix_encoded_array)
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
        msg_matrix_encoded_array = sh.int_matrix_to_bits_matrix(msg_matrix_encoded)
        # 4) Convert 2D array to 1D array
        half_linearize = msg_matrix_encoded_array.ravel()
        # 5) linearize completely
        self.message_to_proc_part = np.empty(0, dtype=np.int8)
        for seq in half_linearize:
            self.message_to_proc_part = np.append(self.message_to_proc_part, seq)
        # 6) Save array length to recover message later
        return len(self.message_to_proc_part)

    def __hide(self, chunk_source, chunk_container):
        """
        Hide message part in container
        :param chunk: chunk to be used as container
        :return:      processed or the original chunk
        """
        container_processed, self.message_to_proc_part = self.gsc.hide(
            chunk_source, chunk_container, self.message_to_proc_part
        )
        if not len(self.message_to_proc_part):
            print(colorize("Message integrated.", COLORS.OKBLUE))
        return chunk_source, container_processed

    def __recover(self, chunk_source, chunk_container):
        """
        Recover message part from container
        :param chunk: container
        """
        message_part = self.gsc.recover(chunk_source, chunk_container)
        if len(message_part):
            # extend msg part array
            self.message_to_proc_part = np.append(
                self.message_to_proc_part, message_part
            )

    # --------------------------------------------------------------
    # Synchronization
    # --------------------------------------------------------------

    def __synchronization_prepare(self):
        """
        Prepare sync text marker to be integrated, by encoding and several additional transforms
        """
        # 1) Transform to matrix
        sync_marker_matrix = sh.message_to_matrix(self.sync_mark)
        # 2) Encode with q-matrix
        sync_marker_matrix_encoded = QMatrix.encode_matrix_message(
            sync_marker_matrix, StegoCore.SYNC_KEY
        )
        # 3) Convert to bits
        sync_marker_matrix_encoded_array = sh.int_matrix_to_bits_matrix(
            sync_marker_matrix_encoded
        )
        # 4) Convert 2D array to 1D array
        half_linearize = sync_marker_matrix_encoded_array.ravel()
        # 5) linearize completely
        self.sync_mark_encoded_array = np.empty(0, dtype=np.int8)
        for seq in half_linearize:
            self.sync_mark_encoded_array = np.append(self.sync_mark_encoded_array, seq)
