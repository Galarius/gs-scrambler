# -*- coding: utf-8 -*-

__author__ = 'Ilya Shoshin'
__copyright__ = 'Copyright 2015, Ilya Shoshin'

import os.path

def load_message_from_file(file_name):
    data = ''
    if os.path.isfile(file_name):
        try:
            with open(file_name, 'r') as f:
                data = f.read()
        except IOError as e:
            print "I/O error({0}): {1}".format(e.errno, e.strerror)
    else:
        print "File doesn't exists!"
    return data


def save_message_to_file(file_name, message):
    if os.path.isfile(file_name):
        try:
            with open(file_name, 'w') as f:
                f.write(message)
        except IOError as e:
            print "I/O error({0}): {1}".format(e.errno, e.strerror)
    else:
        print "File doesn't exists!"


def save_data_to_recover(file_name, message_mediator_length, channel_source, channel_container):
    try:
        with open(file_name, 'w') as f:
            f.write(message_mediator_length)
            f.write(channel_source)
            f.write(channel_container)
    except IOError as e:
        print "I/O error({0}): {1}".format(e.errno, e.strerror)


def load_data_to_recover(file_name):
    message_mediator_length = 0
    channel_source = 0
    channel_container = 0
    if os.path.isfile(file_name):
        try:
            with open(file_name, 'r') as f:
                data = list(f.read())
                message_mediator_length = data[0]
                channel_source = data[1]
                channel_container = data[2]
        except IOError as e:
            print "I/O error({0}): {1}".format(e.errno, e.strerror)
    else:
        print "File doesn't exists!"
    return message_mediator_length, channel_source, channel_container
