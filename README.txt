Spectral Audio Effect MVP v0.2

This package contains complete replacement project/source files, but NOT JUCE itself.
Use it with the existing ~/Documents/SpectralAudioEffect/JUCE submodule.

Features:
- VST3 stereo effect
- 5-second rolling stereo buffer
- up to 24 overlapping Hann-windowed grains
- Grain Size
- Density
- Pitch +/-24 semitones
- Feedback
- Freeze
- Dry/Wet Mix
- parameter state saving
- functional JUCE GUI

Install over existing project:
1. Back up your current source if desired.
2. Copy CMakeLists.txt and Source/ from this ZIP into ~/Documents/SpectralAudioEffect/
3. Clean/rebuild:
   cd ~/Documents/SpectralAudioEffect
   rm -rf build
   cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
   cmake --build build --config Release -j$(nproc)
4. Re-scan/reload Spectral Audio Effect in AudioPluginHost.

Suggested first audible test:
- Mix: 0.65
- Grain Size: 120 ms
- Density: 10 Hz
- Pitch: 0 st
- Feedback: 0
- Freeze: off
Then try Pitch -12 or +7, raise Density, and toggle Freeze after the rolling buffer has captured audio.
