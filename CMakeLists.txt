cmake_minimum_required(VERSION 3.22)
project(SpectralAudioEffect VERSION 0.2.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This ZIP intentionally excludes JUCE. Copy these files into your existing
# ~/Documents/SpectralAudioEffect project, where JUCE/ is already present.
add_subdirectory(JUCE)

juce_add_plugin(SpectralAudioEffect
    COMPANY_NAME "Studorium"
    BUNDLE_ID "com.studorium.spectralaudioeffect"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Stud
    PLUGIN_CODE Spfx
    FORMATS VST3
    PRODUCT_NAME "Spectral Audio Effect")

juce_generate_juce_header(SpectralAudioEffect)

target_sources(SpectralAudioEffect PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h)

target_compile_definitions(SpectralAudioEffect PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(SpectralAudioEffect PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)
