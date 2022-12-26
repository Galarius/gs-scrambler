# gs-scrambler

Streaming audio steganography algorithm developed as the graduation project in Applied Mathematics at Moscow State Technical University of Radio Engineering, Electronics and Automation.

## Features

* Data hiding / extraction in streaming mode.
* Encryption before integration.
* Noise-immune coding based on generalized Fibonacci matrices.
* Using Alter-Johnson shift function as pseudo-random integration scheme generator.
* Resolving "dropped-frames" effect and synchronization problem.
* Estimation of the system with difference and correlation functions provides good results.
* Command-line tool implementing the algorithm was developed with C++ and Python and can be used with any audio system allowing to setup audio input/output.

## Prerequisites (macOS)

* Install `portaudio`

```
brew install portaudio
```

* Create and activate a Python3 virtual environment

```
python3 -mvenv venv  
source ./venv/bin/activate
```

* Install Python dependencies

```
pip install -r requirements.txt
```

* Install PyAudio pointing to portaudio headers and libraries

```
pip install --global-option='build_ext' --global-option='-I/usr/local/include' --global-option='-L/usr/local/lib' pyaudio
```

* Use Cython to generate C code from *.pyx files.

```
cd gs_core && python setup_cy.py build_ext --inplace
cd ..
```

* Configure [settings.json](settings.json)

    * `frames_to_skip` - The amount of audio frames to skip before starting the message injection.

    * `sync_mark` - The unique sequence to be treated as beginning of the message.

    * `frame_size` - The number of frames per buffer.

    * `security_or_capacity`: 
        
        * `0` to insert a message with a semi periodic step (more secure, but requires a larger container),

        * `1` - to integrate message with a fixed step (easier to detect, but more information can be injected, than with `1` option).

    * `build_in_input_audio_device_name`, `build_in_output_audio_device_name`, `virtual_audio_device_name` - Names of audio devices to use.

        *Optionally install [Soundflower](https://github.com/mattingalls/Soundflower) to use as virtual audio device.*

## Usage example 

### 'file to file' mode

```
python gs_scrambler.py -i "wav/input.wav" -m "data/msg.txt" -o "wav/output.wav" -k 7 -r "data/recover_info.txt"
python gs_scrambler.py -i "wav/output.wav" -m "data/msg_recovered.txt" -k 7 -r "data/recover_info.txt"
```

* Interactive streaming mode 

    ```
    python gs_scrambler.py -s
    ```

* Hiding in a fixed container

    ```
    python gs_scrambler.py -i <input_container_file_name> -m <message_file_name> -o <output_container_file_name> -k <key> [-r <recover_info_file_name>]
    ```

* Recovering from the fixed container

    ```
    python gs_scrambler.py -i <input_container_file_name> -m <message_file_name> -k <key> (-l <message_length> or -r <recover_info_file_name>)
    ```


---

## License

See [LICENSE.txt](LICENSE.txt)
