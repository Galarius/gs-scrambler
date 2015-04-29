# -*- coding: utf-8 -*-

"""
Helper methods to detect build-in audio devices indices and SoundFlower virtual device index

PyAudio required.
"""

__author__ = 'Ilya Shoshin'
__copyright__ = 'Copyright 2015, Ilya Shoshin'


def __detect_device_index(p, device_name):
    """
    Detect audio device index
    :param p: PyAudio instance
    :param device_name:
    :return: device index
    """
    idx = -1
    for i in range(p.get_device_count()):
        info = p.get_device_info_by_index(i)
        if info['name'] == device_name:
            idx = info['index']
            break
    return idx


def detect_sound_flower_device_idx(p):
    """
    Detect SoundFlower virtual device index
    :param p: PyAudio instance
    :return: device index
    """
    return __detect_device_index(p, "Soundflower (2ch)")


def detect_build_in_input_device_idx(p):
    """
    Detect Built-in Microph device index
    :param p: PyAudio instance
    :return: device index
    """
    return __detect_device_index(p, "Built-in Microph")


def detect_build_in_output_device_idx(p):
    """
    Detect Built-in Output device index
    :param p: PyAudio instance
    :return: device index
    """
    return __detect_device_index(p, "Built-in Output")


def validate_audio_setup(p, input_format, input_channels, rate, input_device):
    """
    Validate pyaudio stream setup
    :param p: PyAudio instance
    ...
    """
    isSupported = p.is_format_supported(input_format=input_format, input_channels=input_channels, rate=rate,
                                        input_device=input_device)
    return isSupported
