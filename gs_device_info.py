# -*- coding: utf-8 -*-

"""
Helper methods to detect build-in audio devices indices and SoundFlower virtual device index

PyAudio required.
"""

__author__ = 'Ilya Shoshin'
__copyright__ = 'Copyright 2015, Ilya Shoshin'

from gs_settings import StegoSettings

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


def detect_virtual_audio_device_idx(p):
    """
    Detect virtual audio device index
    :param p: PyAudio instance
    :return: device index
    """
    StegoSettings.Instance().deserialize()  # load settings
    return __detect_device_index(p, StegoSettings.Instance().virtual_audio_device_name)


def detect_build_in_input_device_idx(p):
    """
    Detect Built-in input device index
    :param p: PyAudio instance
    :return: device index
    """
    StegoSettings.Instance().deserialize()  # load settings
    return __detect_device_index(p, StegoSettings.Instance().build_in_input_audio_device_name)


def detect_build_in_output_device_idx(p):
    """
    Detect Built-in Output device index
    :param p: PyAudio instance
    :return: device index
    """
    StegoSettings.Instance().deserialize()  # load settings
    return __detect_device_index(p, StegoSettings.Instance().build_in_output_audio_device_name)


def validate_audio_setup(p, input_format, input_channels, rate, input_device):
    """
    Validate pyaudio stream setup
    :param p: PyAudio instance
    ...
    """
    isSupported = p.is_format_supported(input_format=input_format, input_channels=input_channels, rate=rate,
                                        input_device=input_device)
    return isSupported and input_channels > 1
