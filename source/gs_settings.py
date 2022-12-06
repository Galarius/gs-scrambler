# -*- coding: utf-8 -*-

"""
Module to configure application.
"""

__author__ = 'Ilya Shoshin'
__copyright__ = 'Copyright 2015, Ilya Shoshin'

from extensions import Singleton, colorize, COLORS
import os
import json
import gs_device_info

class StreamMode:
    """
    Several stream modes:
        StreamFromBuildInInputToSoundFlower - record sound from mic, hide message and redirect it to SoundFlower virtual device
        StreamFromSoundFlowerToBuildInOutput - receive sound from  SoundFlower virtual device, recover message and play it in dynamics
        StreamFromFileToFile - hide info inside a file on your computer and save it to another file
    """
    StreamFromBuildInInputToSoundFlower = 0
    StreamFromSoundFlowerToBuildInOutput = 1
    StreamFromFileToFile = 2


SETTINGS_FILE_NAME = 'settings.json'            # settings full file name
SETTING_KEY_FRAMES_TO_SKIP = 'frames_to_skip'   # how many frames will be skipped before processing
SETTINGS_KEY_FRAME_SIZE = 'frame_size'          # frame size
SETTINGS_KEY_SYNC_MARK = 'sync_mark'            # synchronization text marker
SETTINGS_SECURITY_OR_CAPACITY = 'security_or_capacity'  # select what is more important
SETTINGS_KEY_BUILD_IN_INPUT = 'build_in_input_audio_device_name'
SETTINGS_KEY_BUILD_IN_OUTPUT = 'build_in_output_audio_device_name'
SETTINGS_KEY_VIRTUAL_DEVICE = 'virtual_audio_device_name'

@Singleton
class StegoSettings:
    """
    Class to store, save and load settings
    """
    def __init__(self):
        self.frames_to_skip = 5
        self.frame_size = 1024
        self.sync_mark = "*()@beg^^^$!7"
        self.security_or_capacity = 0
        self.build_in_input_audio_device_name = "Built-in Input"
        self.build_in_output_audio_device_name = "Built-in Output"
        self.virtual_audio_device_name = "Soundflower (2ch)"

    def serialize(self):
        data = {SETTING_KEY_FRAMES_TO_SKIP: self.frames_to_skip,
                SETTINGS_KEY_FRAME_SIZE: self.frame_size,
                SETTINGS_KEY_SYNC_MARK: self.sync_mark,
                SETTINGS_SECURITY_OR_CAPACITY: self.security_or_capacity,
                SETTINGS_KEY_BUILD_IN_INPUT: self.build_in_input_audio_device_name,
                SETTINGS_KEY_BUILD_IN_OUTPUT: self.build_in_output_audio_device_name,
                SETTINGS_KEY_VIRTUAL_DEVICE: self.virtual_audio_device_name}
        with open(SETTINGS_FILE_NAME, 'w+') as outfile:
            json.dump(data, outfile)

    def deserialize(self, filename=SETTINGS_FILE_NAME):
        if os.path.isfile(filename):
                with open(filename, 'r') as infile:
                    data = json.load(infile)
                    self.frames_to_skip = data[SETTING_KEY_FRAMES_TO_SKIP]
                    self.frame_size = data[SETTINGS_KEY_FRAME_SIZE]
                    self.sync_mark = data[SETTINGS_KEY_SYNC_MARK]
                    self.security_or_capacity = data[SETTINGS_SECURITY_OR_CAPACITY]
                    self.build_in_input_audio_device_name = data[SETTINGS_KEY_BUILD_IN_INPUT]
                    self.build_in_output_audio_device_name = data[SETTINGS_KEY_BUILD_IN_OUTPUT]
                    self.virtual_audio_device_name = data[SETTINGS_KEY_VIRTUAL_DEVICE]
        else:
            print(colorize(f"{SETTINGS_FILE_NAME} couldn't be found. Applying default settings.", COLORS.FAIL))
            self.serialize()
            self.deserialize()

    def validate_stream_mode(self, stream_mode):
        import pyaudio
        p_audio = pyaudio.PyAudio()
        result = False
        if stream_mode == StreamMode.StreamFromFileToFile:
            result = gs_device_info.detect_build_in_input_device_idx(p_audio) >= 0 and \
                    gs_device_info.detect_build_in_output_device_idx(p_audio) >= 0
        elif stream_mode == StreamMode.StreamFromBuildInInputToSoundFlower:
            result = gs_device_info.detect_build_in_input_device_idx(p_audio) >= 0 and \
                     gs_device_info.detect_virtual_audio_device_idx(p_audio) >= 0
        elif stream_mode == StreamMode.StreamFromSoundFlowerToBuildInOutput:
            result = gs_device_info.detect_virtual_audio_device_idx(p_audio) >= 0 and \
                     gs_device_info.detect_build_in_output_device_idx(p_audio) >= 0
        else:
            print(f"Unsupported stream mode! [{stream_mode}]")

        if not result:
            print("Available audio devices:")
            for i in range(p_audio.get_device_count()):
                info = p_audio.get_device_info_by_index(i)
                print(info['name'])
        
        p_audio.terminate()
        return result
