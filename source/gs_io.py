# -*- coding: utf-8 -*-

__author__ = 'Ilya Shoshin'
__copyright__ = 'Copyright 2015, Ilya Shoshin'

import os.path


def load_message_from_file(file_name):
    """
    Loads text file with secret message
    :param file_name:   file name to load
    :return:            data
    """
    data = ''
    if os.path.isfile(file_name):
        try:
            with open(file_name, 'r') as f:
                data = f.read()
        except IOError as e:
            print("I/O error({0}): {1}".format(e.errno, e.strerror))
    else:
        print("File doesn't exists! [%s]"%(file_name))
    return data


def save_message_to_file(file_name, message):
    """
    Saves text file with secret message
    :param file_name:   file name to save
    :param message:   data
    """
    try:
        with open(file_name, 'w+') as f:
            f.write(message)
    except IOError as e:
        print("I/O error({0}): {1}".format(e.errno, e.strerror))


def save_data_to_recover(file_name, message_mediator_length):
    """
    Saves recover info to file
    :param file_name:                   file name
    :param message_mediator_length:     session key
    :return:
    """
    try:
        with open(file_name, 'w+') as f:
            f.write(str(message_mediator_length))
    except IOError as e:
        print("I/O error({0}): {1}".format(e.errno, e.strerror))


def load_data_to_recover(file_name):
    """
    Loads recover info
    :param file_name:
    :return:
    """
    message_mediator_length = 0
    if os.path.isfile(file_name):
        try:
            with open(file_name, 'r') as f:
                message_mediator_length = int(f.read())
        except IOError as e:
            print("I/O error({0}): {1}".format(e.errno, e.strerror))
    else:
        print("File doesn't exists! [%s]"%(file_name))
    return message_mediator_length
