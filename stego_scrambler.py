# -*- coding: utf-8 -*-
#!/usr/bin/env python

"""
StegoScrambler is a steganography scrambler that allows to hide text information inside audio stream.
To perform scrambling this application must be attached to telecommunications application software products,
that provide voice chats. SoundFlower is required to route sound in the write order.

Moreover it is possible to perform just 'file to file' processing if audio container will be send later.
"""

__author__ = 'Ilya Shoshin'
__copyright__ = 'Copyright 2015, Ilya Shoshin'

# Example commands for 'file to file' mode
# python stego_scrambler.py -i "wav/input.wav" -m "data/msg.txt" -o "wav/output.wav" -k 7 -r "data/recover_info.txt"
# python stego_scrambler.py -i "wav/output.wav" -m "data/msg_recovered.txt" -k 7 -r "data/recover_info.txt"

import pyaudio, wave, time

import stego_helper
import io_stego
import stego_device_info

import pyximport; pyximport.install()
from stego_core import StegoCore, StegoMode

from stego_settings import StreamMode, StegoSettings
from extensions import COLORS, colorize

import sys, getopt, cmd, urlparse, inspect
import numpy as np
import math



RECOVER_INFO_DEFAULT_FILE_NAME = 'data/recover_info.txt'
# Keys
KEY_INPUT_FILE_NAME = 'input_key'
KEY_OUTPUT_FILE_NAME = 'output_key'
KEY_STEGO_MODE = 'stego_mode_key'
# Audio setup
CHANNELS = 2
FORMAT = pyaudio.paFloat32
RATE = 44100

class StegoScramblerSession:
    """
    Class to maintain steganography scrambler session.
    """
    def __init__(self, stream_mode, **kwargs):
        """
        Init session
        :param stream_mode:
        :param kwargs:
        :return:
        """
        self.enable_processing = False
        # stream from audio device
        self.stream = None
        # core instance
        self.__configure_for_stream_mode(stream_mode, **kwargs)
        self.core = StegoCore(self.stego_mode, **kwargs)

    def __configure_for_stream_mode(self, stream_mode, **kwargs):
        """
        Configure session for specified stream mode
        :param stream_mode:
        :param kwargs:
        :return:
        """
        print "Configurating..."
        self.stream_mode = stream_mode
        if stream_mode == StreamMode.StreamFromFileToFile:
            # file to file
            input = kwargs[KEY_INPUT_FILE_NAME]
            self.stego_mode = kwargs[KEY_STEGO_MODE]
            self.file_source = wave.open(input, 'rb')
            if self.stego_mode == StegoMode.Hide:
                self.output_wave_file = wave.open(kwargs[KEY_OUTPUT_FILE_NAME], 'wb')
                self.output_wave_file.setnchannels(self.file_source.getnchannels())
                self.output_wave_file.setsampwidth(self.file_source.getsampwidth())
                self.output_wave_file.setframerate(self.file_source.getframerate())
            self.enable_processing = True
        elif stream_mode == StreamMode.StreamFromBuildInInputToSoundFlower:
            # build-in input to sound flower
            self.stego_mode = StegoMode.Hide
        elif stream_mode == StreamMode.StreamFromSoundFlowerToBuildInOutput:
            # sound flower to build-in output
            self.stego_mode = StegoMode.Recover
        else:
            print colorize("Unsupported stream mode! [%i]" % self.stream_mode, COLORS.FAIL)

    @staticmethod
    def is_power2(num):
        "if a number is a power of two"
        return num and not num & (num - 1)

    def __recording_callback(self, in_data, frame_count, time_info, status):
        #-----------------------------------------------------------------------
        if self.stream_mode == StreamMode.StreamFromFileToFile:
            # file to file
            # read frames
            in_data = self.file_source.readframes(frame_count)
            if StegoScramblerSession.is_power2(len(in_data)):
                # decode frames
                left, right = stego_helper.audio_decode(in_data, self.file_source.getnchannels())
                # process frames
                left, right = self.core.process(left, right)
                # encode back
                processed_data = stego_helper.audio_encode((left, right))
            else:
                processed_data = in_data
            # write to file
            if self.stego_mode == StegoMode.Hide:
                self.output_wave_file.writeframes(processed_data)
        #-----------------------------------------------------------------------
        elif self.stream_mode == StreamMode.StreamFromBuildInInputToSoundFlower:
            # build-in input to sound flower or
            if not self.enable_processing:
                # left, right = stego_helper.audio_decode(in_data, SAMPLE_WIDTH, CHANNELS)
                # process frames
                #right = np.array([7 for i in range(1024)], dtype=np.int16)
                #left = np.copy(right)
                # print left, right[0]
                # encode back
                #processed_data = stego_helper.audio_encode((left, right), np_format)
                #return processed_data, pyaudio.paContinue
                return in_data, pyaudio.paContinue

            # decode frames
            left, right = stego_helper.audio_decode(in_data, CHANNELS)
            # process frames
            left, right = self.core.process(left, right)
            # print left, right
            if not len(self.core.message_to_proc_part):
                self.enable_processing = False
            # print n_left.tolist(), n_right[0]
            # encode back
            processed_data = stego_helper.audio_encode((left, right))

        #-----------------------------------------------------------------------
        elif self.stream_mode == StreamMode.StreamFromSoundFlowerToBuildInOutput:
            # sound flower to build-in output
            if not self.enable_processing:
                return in_data, pyaudio.paContinue

            # decode frames
            left, right = stego_helper.audio_decode(in_data, CHANNELS)
            # print len(left), len(right)
            print left, right
            # process frames
            left, right = self.core.process(left, right)
            # encode back
            processed_data = stego_helper.audio_encode((left, right))
        else:
            print 'Unknown mode!'
            processed_data = in_data
        #-----------------------------------------------------------------------
        return processed_data, pyaudio.paContinue

    def open_stream(self):
        # instantiate PyAudio
        self.p_audio = pyaudio.PyAudio()
        if self.stream_mode == StreamMode.StreamFromFileToFile:
            print colorize("Opening file stream...", COLORS.WARNING)
            # file to file
            format, channels, rate = self.p_audio.get_format_from_width(self.file_source.getsampwidth()), \
                                     self.file_source.getnchannels(), self.file_source.getframerate()
            input_dev_idx = stego_device_info.detect_build_in_input_device_idx(self.p_audio)
            output_dev_idx = stego_device_info.detect_build_in_output_device_idx(self.p_audio)
            enable_input, enable_output = False, True
        elif self.stream_mode == StreamMode.StreamFromBuildInInputToSoundFlower:
            print colorize("Opening stream...", COLORS.OKBLUE)
            # build-in input to sound flower
            format, channels, rate = FORMAT, CHANNELS, RATE
            input_dev_idx = stego_device_info.detect_build_in_input_device_idx(self.p_audio)
            output_dev_idx = stego_device_info.detect_sound_flower_device_idx(self.p_audio)
            enable_input, enable_output = True, True
        elif self.stream_mode == StreamMode.StreamFromSoundFlowerToBuildInOutput:
            print colorize("Opening stream...", COLORS.OKGREEN)
            # sound flower to build-in output
            format, channels, rate = FORMAT, CHANNELS, RATE
            input_dev_idx = stego_device_info.detect_sound_flower_device_idx(self.p_audio)
            output_dev_idx = stego_device_info.detect_build_in_output_device_idx(self.p_audio)
            enable_input, enable_output = True, True
        else:
            print colorize("Unsupported stream mode! [%i]" % self.stream_mode, COLORS.FAIL)
            raise ValueError("Unsupported stream mode! [%i]" % self.stream_mode)

        settings = StegoSettings.Instance()

        print colorize("Input device: %i" % input_dev_idx, COLORS.WARNING)
        print colorize("Output device: %i" % output_dev_idx, COLORS.WARNING)
        print colorize("Format: {0}, Channels: {1}, Rate: {2}, Frame size: {3}".format(format, channels, rate,
                                                                                settings.frame_size), COLORS.WARNING)

        # standard L-R stereo
        channel_map = (0, 1)

        try:
            stream_info = pyaudio.PaMacCoreStreamInfo(
                flags=pyaudio.PaMacCoreStreamInfo.paMacCorePlayNice,  # default
                channel_map=channel_map)
        except AttributeError:
            print colorize("Sorry, couldn't find PaMacCoreStreamInfo. Make sure that "
                  "you're running on Mac OS X.", COLORS.FAIL)
            sys.exit(-1)

        if stego_device_info.validate_audio_setup(self.p_audio, format, channels, rate, input_dev_idx):
            self.stream = self.p_audio.open(format=format,
                                            channels=channels,
                                            rate=rate,
                                            frames_per_buffer=settings.frame_size,
                                            input=enable_input,
                                            output=enable_output,
                                            output_host_api_specific_stream_info=stream_info,
                                            input_device_index=input_dev_idx,
                                            output_device_index=output_dev_idx,
                                            stream_callback=self.__recording_callback)
            self.stream.start_stream()

            src_latency = 1000.0 * self.stream.get_input_latency()
            buffer_latency = 1000.0 * settings.frame_size / RATE
            dst_latency = 1000.0 * self.stream.get_output_latency()
            total_latency = buffer_latency + dst_latency + src_latency
            print colorize("Expected delay: %0.1f ms (src: %0.1f, buf: %0.1f, dst: %0.1f)" % (
                    total_latency, src_latency, buffer_latency, dst_latency), COLORS.WARNING)
        else:
            print colorize("Unsupported audio configuration!", COLORS.FAIL)
            raise ValueError("Unsupported audio configuration!")

    def close_stream(self):
        print "Closing stream..."
        if self.stream:
            self.stream.stop_stream()
            self.stream.close()

        if self.stream_mode == StreamMode.StreamFromFileToFile:
            # file to file
            self.file_source.close()
            if self.stego_mode == StegoMode.Hide:
                self.output_wave_file.close()

        self.p_audio.terminate()

    def hide(self, msg_file_name, key, recover_info_filename=RECOVER_INFO_DEFAULT_FILE_NAME):
        """
        Load message and call core method to begin integration.
        :param msg_file_name: file name of the file with message
        :param key:           key to encode message
        """
        if not (self.stream_mode == StreamMode.StreamFromBuildInInputToSoundFlower or
                self.stream_mode == StreamMode.StreamFromFileToFile):
            print colorize("Not applicable for current stream mode!", COLORS.FAIL)
        # load mesage
        message = io_stego.load_message_from_file(msg_file_name)
        if not message == '':
            print colorize("Begin integration...", COLORS.OKBLUE)
            session_key = self.core.hide(message, key)
            io_stego.save_data_to_recover(recover_info_filename, session_key)
            self.enable_processing = True
        else:
            print colorize("Wrong or empty file with message!", COLORS.FAIL)

    def scan(self):
        """
        Begin synchronization and message extraction.
        """
        if not (self.stream_mode == StreamMode.StreamFromSoundFlowerToBuildInOutput or
                self.stream_mode == StreamMode.StreamFromFileToFile):
            print colorize("Not applicable for current stream mode!", COLORS.FAIL)
            return
        print colorize("Begin scanning...", COLORS.OKGREEN)
        self.enable_processing = True

    def recover(self, session_key, user_key, msg_file_name):
        """
        Extract and decode message, save it to file
        :param session_key:
        :param user_key:
        :return:
        """
        if not (self.stream_mode == StreamMode.StreamFromSoundFlowerToBuildInOutput or
                self.stream_mode == StreamMode.StreamFromFileToFile):
            print colorize("Not applicable for current stream mode!", COLORS.FAIL)
            return
        message = self.core.recover(session_key, user_key)
        io_stego.save_message_to_file(msg_file_name, message)
        if not message == '':
            print colorize("Message recovered.", COLORS.OKGREEN)


def print_usage():
    print """

            Start interactive prompt:
                 ----------------------------
                 python stego_scrambler.py -s
                 ----------------------------
            or process .wav file directly:
                *) hide msg
                    ------------------------------
                    stego_scrambler.py -i <input_container_file_name> -m <message_file_name> -o <output_container_file_name> -k <key> [-r <recover_info_file_name>]
                    ------------------------------
                *) recover msg
                    ------------------------------
                    stego_scrambler.py -i <input_container_file_name> -m <message_file_name> -k <key> (-l <message_length> or -r <recover_info_file_name>)
                    ------------------------------
          """

def main(opts):

    input_container_file_name = ''
    message_file_name = ''
    output_container_file_name = ''
    key = -1
    message_length = -1
    recover_info_file_name = RECOVER_INFO_DEFAULT_FILE_NAME
    # --------------------------------------
    for opt, arg in opts:
        if opt == '-h':
            print_usage()
            sys.exit()
        elif opt in ("-i", "--ifile"):
            input_container_file_name = arg
        elif opt in ("-m", "--mfile"):
            message_file_name = arg
        elif opt in ("-o", "--ofile"):
            output_container_file_name = arg
        elif opt in ("-k", "--key"):
            key = int(arg)
        elif opt in ("-l", "--length"):
            message_length = int(arg)
        elif opt in ("-r", "--recover_info"):
            recover_info_file_name = arg

    if input_container_file_name == '' or message_file_name == '':
        print_usage()
        sys.exit()

    if not output_container_file_name == '':
        # --------------------------------------
        # hide
        # --------------------------------------
        # load settings
        settings = StegoSettings.Instance()
        settings.deserialize()
        if settings.validate_stream_mode(StreamMode.StreamFromFileToFile):
            stego_session = StegoScramblerSession(StreamMode.StreamFromFileToFile, **{
                                                    KEY_STEGO_MODE:StegoMode.Hide,
                                                    KEY_INPUT_FILE_NAME:input_container_file_name,
                                                    KEY_OUTPUT_FILE_NAME:output_container_file_name,
                                                    StegoCore.SKIP_FRAMES_KEY: settings.frames_to_skip,
                                                    StegoCore.SYNC_MARK_KEY: settings.sync_mark
                                                    })
            stego_session.open_stream()
            stego_session.hide(message_file_name, key)
        else:
            print "There are no supported audio devices for current stream mode."
        # --------------------------------------
    else:
        if message_length <= 0:
            message_length = io_stego.load_data_to_recover(recover_info_file_name)
        if message_length > 0:
            # --------------------------------------
            # recover
            # --------------------------------------
            # load settings
            settings = StegoSettings.Instance()
            settings.deserialize()
            if settings.validate_stream_mode(StreamMode.StreamFromFileToFile):
                stego_session = StegoScramblerSession(StreamMode.StreamFromFileToFile, **{
                                                    KEY_STEGO_MODE:StegoMode.Recover,
                                                    KEY_INPUT_FILE_NAME:input_container_file_name,
                                                    StegoCore.SKIP_FRAMES_KEY: settings.frames_to_skip,
                                                    StegoCore.SYNC_MARK_KEY: settings.sync_mark
                                                    })
                stego_session.open_stream()
                stego_session.scan()
            else:
                print "There are no supported audio devices for current stream mode."
            # --------------------------------------
        else:
            print_usage()
            sys.exit()

    try:
        while stego_session.stream.is_active():
            time.sleep(0.1)
    except KeyboardInterrupt:
        pass
    finally:
        if stego_session.stego_mode == StegoMode.Recover:
            session_key = io_stego.load_data_to_recover(recover_info_file_name)
            stego_session.recover(session_key, key, message_file_name)
        stego_session.close_stream()
        print colorize('Done!', COLORS.WARNING)
    sys.exit(0)


class InteractiveStegoScrambler(cmd.Cmd):
    """
    Interactive prompt for steganography scrambler.
    """

    prompt = 'scrambler prompt: '
    intro = "Interactive prompt for steganography scrambler."

    def __init__(self):
        cmd.Cmd.__init__(self)
        self.session = None
        print colorize("Welcome!", COLORS.WARNING)

    def print_err(self, fa_name):
        print colorize("Wrong arguments. Type `help " + fa_name + "`", COLORS.FAIL)

    def do_connect(self, line):
        """
        Create session with specified mode.

        usage:
            connect m=<mode>

        :param mode:
            0 - record sound from mic, hide message and redirect it to SoundFlower virtual device
            1 - receive sound from  SoundFlower virtual device, recover message and play it in dynamics
        """
        args = urlparse.parse_qs(line)
        mode = int(args['m'][0]) if len(args.keys()) > 0 else 0

        if not (mode == StreamMode.StreamFromBuildInInputToSoundFlower or \
                mode == StreamMode.StreamFromSoundFlowerToBuildInOutput):
            self.print_err(inspect.currentframe().f_code.co_name.replace('do_',''))
            return

        # load settings
        settings = StegoSettings.Instance()
        settings.deserialize()
        if settings.validate_stream_mode(mode):
            self.session = StegoScramblerSession(mode, **{StegoCore.SYNC_MARK_KEY: settings.sync_mark})
            self.session.open_stream()
        else:
            print colorize("There are no supported audio devices for current stream mode.", COLORS.FAIL)

    def do_hide(self, line):
        """
        Hide message with key.

        usage:
            hide f=<msg filename>&k=<key>

        :param msg_file_name: path to the text file with message
        :param key: key to encode message (unsigned integer)
        """
        #args = urlparse.parse_qs(line)

        #try:
        #    msg_file_name, key = args['f'][0], int(args['k'][0])
        #    if msg_file_name.strip() == '' or key <= 0:
                #self.print_err(inspect.currentframe().f_code.co_name.replace('do_', ''))
                #return
        msg_file_name = 'data/msg.txt'
        key = 7
        #except KeyError:
        #    self.print_err(inspect.currentframe().f_code.co_name.replace('do_', ''))
        #    return

        self.session.hide(msg_file_name, key)

    def do_scan(self, line):
        """
        Scan stream for synchronization marker and begin message extraction when synchronized.
        """
        self.session.scan()

    def do_recover(self, line):
        """
        Recover extracted message with keys. Should be called after sinchronizaion, often after voice chat is over.

        usage:
            recover s=<session key>&k=<key>&f=<msg filename>

        :param session_key: key to recover message  (unsigned integer)
        :param user_key: key to decode message      (unsigned integer)
        :param msg_file_name: name of the file to save message to
        """
        args = urlparse.parse_qs(line)
        try:
            session_key, user_key, msg_file_name = int(args['s'][0]), int(args['k'][0]), args['f'][0]
            if msg_file_name.strip() == '' or user_key <= 0 or session_key <= 0:
                self.print_err(inspect.currentframe().f_code.co_name.replace('do_', ''))
                return
        except KeyError:
            self.print_err(inspect.currentframe().f_code.co_name.replace('do_', ''))
            return
        self.session.recover(session_key, user_key, msg_file_name)

    def do_disconnect(self, line):
        """
        Disconnect scrambler.
        """
        if self.session:
            self.session.close_stream()
        sys.exit()

    def do_EOF(self, line):
        return True

    def cmdloop(self):
        try:
            cmd.Cmd.cmdloop(self)
        except KeyboardInterrupt as e:
            self.do_disconnect('')


if __name__ == "__main__":
    argv = sys.argv[1:]
    try:
       opts, args = getopt.getopt(argv, "hsi:m:o:k:l:r:", ["ifile=", "mfile=", "ofile=", "key=", "length=", "recover_info="])
    except getopt.GetoptError:
       print_usage()
       sys.exit(2)

    if not len(opts):
        print_usage()
        sys.exit(1)

    for opt, arg in opts:
        if opt == '-h':
            print_usage()
            sys.exit()
        if opt == '-s':
            # run interactive prompt
            InteractiveStegoScrambler().cmdloop()
        else:
            # run automatic mode (for direct file processing)
            main(opts)