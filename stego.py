# -*- coding: utf-8 -*-
#!/usr/bin/env python

__author__ = 'Ilya Shoshin'
__copyright__ = 'Copyright 2015, Ilya Shoshin'


import pyaudio
import time
import sys
import wave
import stego_helper
import stego_core as sc


WAVE_INPUT_FILENAME = "input.wav"
WAVE_OUTPUT_FILENAME = "output.wav"


class StegoSession:
    def __init__(self, p_audio, message, key, stego_mode):
        self.p_audio = p_audio
        self.stream = None
        self.stego_mode = stego_mode
        self.core = sc.StegoCore(message, key, stego_mode)

        input = WAVE_INPUT_FILENAME if stego_mode == sc.StegoMode.Hide else WAVE_OUTPUT_FILENAME
        self.file_source = wave.open(input, 'rb')
        if stego_mode == sc.StegoMode.Hide:
            self.output_wave_file = wave.open(WAVE_OUTPUT_FILENAME, 'wb')
            self.output_wave_file.setnchannels(self.file_source.getnchannels())
            self.output_wave_file.setsampwidth(self.file_source.getsampwidth())
            self.output_wave_file.setframerate(self.file_source.getframerate())


    def recording_callback(self, in_data, frame_count, time_info, status):
        in_data = self.file_source.readframes(frame_count)

        if len(in_data) >= 1024:    # temp
            left, right = stego_helper.audio_decode(in_data, int(len(in_data) / 2.0), self.file_source.getnchannels())
            left = self.core.process(left)
            # write back
            processed_data = stego_helper.audio_encode((left, right), self.file_source.getnchannels())
        else:
            processed_data = in_data

        if self.stego_mode == sc.StegoMode.Hide:
            self.output_wave_file.writeframes(processed_data)

        return processed_data, pyaudio.paContinue

    def open_stream(self):

        self.stream = self.p_audio.open(format=self.p_audio.get_format_from_width(self.file_source.getsampwidth()),
                                        channels=self.file_source.getnchannels(),
                                        rate=self.file_source.getframerate(),
                                        #input=True,
                                        output=True,
                                        stream_callback=self.recording_callback)
        self.stream.start_stream()

    def close_stream(self):
        self.stream.stop_stream()
        self.stream.close()
        self.file_source.close()
        if self.stego_mode == sc.StegoMode.Hide:
            self.output_wave_file.close()


def main(argv):
    p = pyaudio.PyAudio()
    msg = "Hello, stego world!"
    key = 1
    stego_session = StegoSession(p, msg, key, sc.StegoMode.Hide)
    #stego_session = StegoSession(p, msg, key, sc.StegoMode.Recover)
    stego_session.open_stream()
    try:
        while stego_session.stream.is_active():
            time.sleep(0.1)
    except KeyboardInterrupt:
        pass
    finally:
        stego_session.close_stream()
        print stego_session.core.recover_message()
        p.terminate()
    sys.exit(0)

if __name__ == "__main__":
    main(sys.argv[1:])