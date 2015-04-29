# -*- coding: utf-8 -*-
#!/usr/bin/env python
# python stego.py -i "wav/input.wav" -m "data/msg.txt" -o "wav/output.wav" -k 7 -r "data/recover_info.txt"
# python stego.py -i "wav/output.wav" -m "data/msg_recovered.txt" -k 7 -r "data/recover_info.txt"

__author__ = 'Ilya Shoshin'
__copyright__ = 'Copyright 2015, Ilya Shoshin'


import pyaudio
import time
import sys, getopt
import wave
import stego_helper
import stego_core as sc
import io_stego
import stego_device_info
import stego_settings


class StegoSession:

    INPUT_FILE_NAME_KEY = 'input'
    OUTPUT_FILE_NAME_KEY = 'output'

    def __init__(self, p_audio, stego_mode, key, **kwargs):
        self.p_audio = p_audio
        self.stream = None
        self.stego_mode = stego_mode
        self.core = sc.StegoCore(stego_mode, key, **kwargs)

        input = kwargs[StegoSession.INPUT_FILE_NAME_KEY]
        self.file_source = wave.open(input, 'rb')
        if stego_mode == sc.StegoMode.Hide:
            self.output_wave_file = wave.open(kwargs[StegoSession.OUTPUT_FILE_NAME_KEY], 'wb')
            self.output_wave_file.setnchannels(self.file_source.getnchannels())
            self.output_wave_file.setsampwidth(self.file_source.getsampwidth())
            self.output_wave_file.setframerate(self.file_source.getframerate())


    def recording_callback(self, in_data, frame_count, time_info, status):
        """
        :param in_data:
        :param frame_count:
        :param time_info:
        :param status:
        :return:
        """

        settings = stego_settings.StegoSettings.Instance()

        if settings.stream_mode == stego_settings.StreamMode.StreamFromFileToFile:
            # read frames
            in_data = self.file_source.readframes(frame_count)
            # decode frames
            left, right = stego_helper.audio_decode(in_data, int(len(in_data) / 2.0), self.file_source.getnchannels())
            # process frames
            right = self.core.process(left, right)
            # encode back
            processed_data = stego_helper.audio_encode((left, right), self.file_source.getnchannels())
            # write back
            if self.stego_mode == sc.StegoMode.Hide:
                self.output_wave_file.writeframes(processed_data)
        elif settings.stream_mode == stego_settings.StreamMode.StreamFromBuildInInputToSoundFlower or \
             settings.stream_mode == stego_settings.StreamMode.StreamFromSoundFlowerToBuildInOutput:
            # read frames
            in_data = self.stream.readframes(frame_count)
            # decode frames
            left, right = stego_helper.audio_decode(in_data, int(len(in_data) / 2.0), self.file_source.getnchannels())
            # process frames
            right = self.core.process(left, right)
            # encode back
            processed_data = stego_helper.audio_encode((left, right), self.file_source.getnchannels())
            # write back
            self.stream.writeframes(processed_data)

        return processed_data, pyaudio.paContinue

    def open_stream(self):

        settings = stego_settings.StegoSettings.Instance()

        if settings.stream_mode == stego_settings.StreamMode.StreamFromFileToFile:
            input_dev_idx = stego_device_info.detect_build_in_input_device_idx(self.p_audio)
            output_dev_idx = stego_device_info.detect_build_in_output_device_idx(self.p_audio)
            enable_input=False
            enable_output=True
        elif settings.stream_mode == stego_settings.StreamMode.StreamFromBuildInInputToSoundFlower:
            input_dev_idx = stego_device_info.detect_build_in_input_device_idx(self.p_audio)
            output_dev_idx = stego_device_info.detect_sound_flower_device_idx(self.p_audio)
            enable_input=True
            enable_output=False
        elif settings.stream_mode == stego_settings.StreamMode.StreamFromSoundFlowerToBuildInOutput:
            input_dev_idx = stego_device_info.detect_sound_flower_device_idx(self.p_audio)
            output_dev_idx = stego_device_info.detect_build_in_output_device_idx(self.p_audio)
            enable_input=False
            enable_output=True
        else:
            print "Unsupported stream mode! [%i]" % self.stream_mode

        self.stream = self.p_audio.open(format=self.p_audio.get_format_from_width(self.file_source.getsampwidth()),
                                        channels=self.file_source.getnchannels(),
                                        rate=self.file_source.getframerate(),
                                        input=enable_input,
                                        output=enable_output,
                                        input_device_index = input_dev_idx,
                                        output_device_index = output_dev_idx,
                                        stream_callback=self.recording_callback)
        self.stream.start_stream()

    def close_stream(self):
        self.stream.stop_stream()
        self.stream.close()
        self.file_source.close()
        if self.stego_mode == sc.StegoMode.Hide:
            self.output_wave_file.close()


def print_usage():
    print """
            hide msg:    stego.py -i <input_container_file_name> -m <message_file_name> -o <output_container_file_name> -k <key> [-r <recover_info_file_name>]
            recover msg: stego.py -i <input_container_file_name> -m <message_file_name> -k <key> (-l <message_length> or -r <recover_info_file_name>)
          """


def main(argv):

    input_container_file_name = ''
    message_file_name = ''
    output_container_file_name = ''
    key = -1
    message_length = -1
    recover_info_file_name = 'data/recover_info.txt'
    # --------------------------------------
    try:
       opts, args = getopt.getopt(argv, "hi:m:o:k:l:r:", ["ifile=", "mfile=", "ofile=", "key=", "length=", "recover_info="])
    except getopt.GetoptError:
       print_usage()
       sys.exit(2)
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
        # load mesage
        message = io_stego.load_message_from_file(message_file_name)
        if not message == '':
            p = pyaudio.PyAudio()
            # load settings
            settings = stego_settings.StegoSettings.Instance()
            if settings.validate_stream_mode(p):
                stego_session = StegoSession(p, sc.StegoMode.Hide, key, **{sc.StegoCore.MESSAGE_KEY:message,
                                                        StegoSession.INPUT_FILE_NAME_KEY:input_container_file_name,
                                                        sc.StegoCore.SKIP_FRAMES_KEY: 10,
                                                        StegoSession.OUTPUT_FILE_NAME_KEY:output_container_file_name})
            else:
                print "There are no supported audio devices for current stream mode."
        else:
            print "Wrong or empty file with message!"
            print_usage()
            sys.exit()
        # --------------------------------------
    else:
        if message_length <= 0:
            message_length = io_stego.load_data_to_recover(recover_info_file_name)
        if message_length > 0:
            # --------------------------------------
            # recover
            # --------------------------------------
            p = pyaudio.PyAudio()
            # load settings
            settings = stego_settings.StegoSettings.Instance()
            settings.deserialize()
            if settings.validate_stream_mode(p):
                stego_session = StegoSession(p, sc.StegoMode.Recover, key, **{sc.StegoCore.LENGTH_KEY:message_length,
                                                                          sc.StegoCore.SKIP_FRAMES_KEY: 10,
                                                        StegoSession.INPUT_FILE_NAME_KEY:input_container_file_name})
            else:
                print "There are no supported audio devices for current stream mode."
            # --------------------------------------
        else:
            print_usage()
            sys.exit()

    stego_session.open_stream()

    try:
        while stego_session.stream.is_active():
            time.sleep(0.1)
    except KeyboardInterrupt:
        pass
    finally:
        stego_session.close_stream()
        if stego_session.stego_mode == sc.StegoMode.Recover:
            io_stego.save_message_to_file(message_file_name, stego_session.core.recover_message())
        else:
            io_stego.save_data_to_recover(recover_info_file_name, stego_session.core.mediate_length)
        p.terminate()
        print 'Done!'
    sys.exit(0)

if __name__ == "__main__":
    main(sys.argv[1:])