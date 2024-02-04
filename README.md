# Syntheloper

Syntheloper is a C++ synthesizer library based on portAudio.h.

## Overview of the Architecture

### Synth Engine

The Syntheloper object creates a callback thread that feeds 44100 floats to the sound card per second. Within this callback thread, it iterates through all the unit generators (Ugens) and UgenLinks (message passers between Ugens), calling their tick function.

### WaveTable and Game Engine-styled Resource Management

Users can define any waveform as a wavetable object, including sine waves, phasors, or reading from file assets (an upcoming feature). The resource management ensures that each waveform is unique through runtime. For example, if a user needs 100 OSCs, only one sine waveform object is allocated in memory.

### Ugens

Ugen holds synth-related attributes and can tick themselves according to them for time-varying output signals. The current two children of the Ugen class are Osc and ADSRenv. Osc holds frequency, amplitude, phase, and a pointer to a user-defined wavetable. ADSR contains two 2D vectors for forming an ADSR wave shape.

### Pooling

Refer to src/memory/README.md for details. It involves preallocated memory pooling.

### Message Passing via Shared Memory: UgenLinker

Syntheloper achieves Ugen-wise communication via Linker. In the case where one Ugen needs to send and receive signals, the Ugen will allocate a channel grain on the memory pool, storing their elected attributes (for example, OSC has frequency, amplitude, phase, and outputs from tick stored). When both sender and receiver Ugen have their channel grain uploaded, UgenLinker will read the sender's memory location and write to the receiver's memory location with user-defined modifications (Ugenlinker takes a function pointer as an alternative attribute in the case of FLOPs needing to be performed before communicating).

### Synthesizer Example: Modular Synth

Users can modify the Syntheloper::customSetup() function for different features. The current custom setup achieves a modular synthesizer:

- First, initialize an OSC with the frequency set as MIDI(60) and amplitude=0.5. Set it to use a sinusoidal wavetable by passing a function pointer in the constructor. The wavetable is tagged with the name "sine".
- Declare another OSC with frequency=1 and amplitude=0.5. Now only set the name tag, "sine", of the wavetable, but not the function pointer, as there is already one sine wave table allocated by the previous OSC.
- Raise the second OSC (the modifier)'s output by its amplitude and divide the result by 2. Let the modifier's output modify the carrier's amplitude to perform AM synthesis. One can manage to create an FM synth by simply altering some figures of the above setup.

## Upcoming Features

- Delay buffer
- Filters
- Class Instrument: Ugen
- Puredata's bang
- Lockfree Ring buffer
- Sequencer

## Installation and Play

Only tested on x64 Windows systems. Make sure to install portaudio.h. I recommend using vcpkg for installing this library and Visual Studio 2022 for playing with the solution.