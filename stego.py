"""
PyAudio Example: Make a wire between input and output (i.e., record a
few samples and play them back immediately).

This is the callback (non-blocking) version.
"""

import pyaudio
import time
import sys
import wave
import stego_helper
import q_matrix as qm

CHUNK = 1024
FORMAT = pyaudio.paInt16
CHANNELS = 2
RATE = 44100
RECORD_SECONDS = 5
WAVE_INPUT_FILENAME = "input.wav"
WAVE_OUTPUT_FILENAME = "output.wav"


class StegoMode:
    Encode = 0,
    Decode = 1


class StreamMode:
    RecordAndPlay = 0,
    RecordAndWrite = 1

class StreamSource:
    Microphone = 0,
    File = 1


class StegoSession:
    def __init__(self, p_audio, stream_mode, stream_source):
        self.p_audio = p_audio
        self.stream_mode = stream_mode
        self.stream_source = stream_source
        self.stream = None

        if self.stream_source == StreamSource.File:
            self.file_source = wave.open(WAVE_INPUT_FILENAME, 'rb')

            if self.stream_mode == StreamMode.RecordAndWrite:
                self.output_wave_file = wave.open(WAVE_OUTPUT_FILENAME, 'wb')
                self.output_wave_file.setnchannels(self.file_source.getnchannels())
                self.output_wave_file.setsampwidth(self.file_source.getsampwidth())
                self.output_wave_file.setframerate(self.file_source.getframerate())
        else:
            if self.stream_mode == StreamMode.RecordAndWrite:
                self.output_wave_file = wave.open(WAVE_OUTPUT_FILENAME, 'wb')
                self.output_wave_file.setnchannels(CHANNELS)
                self.output_wave_file.setsampwidth(self.p_audio.get_sample_size(FORMAT))
                self.output_wave_file.setframerate(RATE)

    def recording_callback(self, in_data, frame_count, time_info, status):
        #if self.stream_source == StreamSource.File:

        in_data = self.file_source.readframes(frame_count)

        left, right = stego_helper.audio_decode(in_data, int(len(in_data)/2.0), self.file_source.getnchannels())

        stego_helper.jonson(left)
        #c = [stego_helper.d_2_b(x) for x in left]
        #print c[0], c[1]

        processed_data = stego_helper.audio_encode((left, right), self.file_source.getnchannels())

        self.output_wave_file.writeframes(processed_data)

        #if self.stream_mode == StreamMode.RecordAndWrite:
        #    self.output_wave_file.writeframes(in_data)
        #elif self.stream_mode == StreamMode.RecordAndPlay:
        #    self.stream.write(in_data, frame_count)

        return processed_data, pyaudio.paContinue

    def open_stream(self):

        if self.stream_source == StreamSource.File:
            print "file as source"
            self.stream = self.p_audio.open(format=self.p_audio.get_format_from_width(self.file_source.getsampwidth()),
                                            channels=self.file_source.getnchannels(),
                                            rate=self.file_source.getframerate(),
                                            # input=True,
                                            output=True, #(self.stream_mode == StreamMode.RecordAndPlay),
                                            stream_callback=self.recording_callback)
        else:
            self.stream = self.p_audio.open(format=FORMAT,
                                            channels=CHANNELS,
                                            rate=RATE,
                                            input=True,
                                            output=(self.stream_mode == StreamMode.RecordAndPlay),
                                            frames_per_buffer=CHUNK,
                                            stream_callback=self.recording_callback)

        self.stream.start_stream()

    def close_stream(self):
        self.stream.stop_stream()
        self.stream.close()
        if self.stream_mode == StreamMode.RecordAndWrite:
            self.output_wave_file.close()
        if self.stream_source == StreamSource.File:
            self.file_source.close()


def main(argv):
    p = pyaudio.PyAudio()
    stego_session = StegoSession(p, StreamMode.RecordAndWrite, StreamSource.File)
    stego_session.open_stream()
    try:
        while stego_session.stream.is_active():
            time.sleep(0.1)
    except KeyboardInterrupt:
        pass
    finally:
        stego_session.close_stream()
        p.terminate()
    sys.exit(0)

if __name__ == "__main__":
    main(sys.argv[1:])