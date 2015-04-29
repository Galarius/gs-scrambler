# -*- coding: utf-8 -*-

"""
Module to configure application.
"""

__author__ = 'Ilya Shoshin'
__copyright__ = 'Copyright 2015, Ilya Shoshin'

from extensions import Singleton
import os
import json
import stego_device_info

class StreamMode:
    """
    Several stream modes:
        StreamFromFileToFile - hide info inside a file on your computer and save it to another file
        StreamFromBuildInInputToSoundFlower - record sound from mic, hide message and redirect it to SoundFlower virtual device
        StreamFromSoundFlowerToBuildInOutput - receive sound from  SoundFlower virtual device, recover message and play it in dynamics
    """
    StreamFromFileToFile = 0
    StreamFromBuildInInputToSoundFlower = 1
    StreamFromSoundFlowerToBuildInOutput = 2


SETTINGS_FILE_NAME = 'settings.json'            # settings full file name
SETTING_KEY_FRAMES_TO_SKIP = 'frames_to_skip'   # how many frames will be skipped before processing
SETTING_KEY_STREAM_MODE = 'stream_mode'         # which stream mode will be used

@Singleton
class StegoSettings:
    """
    Class to store, save and load settings
    """
    def __init__(self):
        self.frames_to_skip = 5
        self.stream_mode = StreamMode.StreamFromFileToFile

    def serialize(self):
        data = {SETTING_KEY_FRAMES_TO_SKIP: self.frames_to_skip,
                SETTING_KEY_STREAM_MODE: self.stream_mode}
        with open(SETTINGS_FILE_NAME, 'w+') as outfile:
            json.dump(data, outfile)

    def deserialize(self):
        if os.path.isfile(SETTINGS_FILE_NAME):
                with open(SETTINGS_FILE_NAME, 'r') as infile:
                    data = json.load(infile)
                    self.frames_to_skip = int(data[SETTING_KEY_FRAMES_TO_SKIP])
                    self.stream_mode = data[SETTING_KEY_STREAM_MODE]
        else:
            self.serialize()
            self.deserialize()

    def validate_stream_mode(self, p_audio):
        if self.stream_mode == StreamMode.StreamFromFileToFile:
            return stego_device_info.detect_build_in_input_device_idx(p_audio) >= 0 and \
                   stego_device_info.detect_build_in_output_device_idx(p_audio) >= 0
        elif self.stream_mode == StreamMode.StreamFromBuildInInputToSoundFlower:
            return stego_device_info.detect_build_in_input_device_idx(p_audio) >= 0 and \
                   stego_device_info.detect_sound_flower_device_idx(p_audio) >= 0
        elif self.stream_mode == StreamMode.StreamFromSoundFlowerToBuildInOutput:
            return stego_device_info.detect_sound_flower_device_idx(p_audio) >= 0 and \
                   stego_device_info.detect_build_in_output_device_idx(p_audio) >= 0
        else:
            print "Unsupported stream mode! [%i]" % self.stream_mode
