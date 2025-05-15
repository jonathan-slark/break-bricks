/*
 * aud.c - Audio Subsystem Implementation
 *
 * This file implements the audio system for the project using the miniaudio library.
 * It provides functions for initializing and terminating the audio engine, as well as
 * loading, playing, and unloading sound assets. The audio engine is configured with
 * modern settings: stereo output (2 channels) and a sample rate of 48000 Hz.
 *
 * Key Features:
 *   - Initialization of the miniaudio engine with user-defined volume settings.
 *   - Preloading and decoding of sound files to minimize runtime overhead during gameplay.
 *   - Simple APIs to start and stop sound playback.
 *   - Clean shutdown of the audio engine to free resources.
 *
 * Dependencies:
 *   - miniaudio (https://github.com/mackron/miniaudio)
 *   - aud.h and main.h (for function declarations and error handling routines)
 *
 * Usage:
 *   - Call aud_init(vol) to initialize the audio engine.
 *   - Use aud_loadSound() to preload sounds for optimized playback.
 *   - Use aud_playSound() for immediate sound playback.
 *   - Call aud_term() to properly shut down the audio engine.
 *
 * Author: Jonathan Slark
 * Date:   04/04/2025
 */

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "aud.h"
#include "main.h"

// Constants
static const ma_uint32 CHANNELS    = 2;
static const ma_uint32 SAMPLE_RATE = 48000;

// Variables
ma_engine engine;

// Function definitions

void aud_init(float vol)
{
    ma_engine_config ec;
    ec = ma_engine_config_init();
    ec.channels   = CHANNELS;
    ec.sampleRate = SAMPLE_RATE;
    if (ma_engine_init(&ec, &engine) != MA_SUCCESS) {
	main_term(EXIT_FAILURE, "Failed to initialise audio engine.\n");
    }
    ma_engine_set_volume(&engine, vol);
}

void aud_term(void)
{
    ma_engine_uninit(&engine);
}

// Preload sound
// https://github.com/mackron/miniaudio/issues/249
ma_sound* aud_loadSound(const char* file, bool isLooping)
{
    ma_sound* sound = (ma_sound*) malloc(sizeof(ma_sound));

    // Load and decode now to avoid overhead during game play
    // https://miniaud.io/docs/manual/index.html#OptimizationTips
    if (ma_sound_init_from_file(&engine, file,
		MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_PITCH | MA_SOUND_FLAG_NO_SPATIALIZATION,
		NULL, NULL, sound) != MA_SUCCESS) {
	main_term(EXIT_FAILURE, "Unable to load sound %s.\n", file);
    }

    ma_sound_set_looping(sound, isLooping);

    return sound;
}

void aud_unloadSound(ma_sound *sound)
{
    ma_sound_uninit(sound);
    free(sound);
}

// Will use cached sound data and not actually load sound again
void aud_playSound(const char* file)
{
    ma_engine_play_sound(&engine, file, nullptr);
}

// Stream music
ma_sound* aud_playMusic(const char* file, bool isLooping)
{
    ma_sound* music = (ma_sound*) malloc(sizeof(ma_sound));
    if (ma_sound_init_from_file(&engine, file,
                MA_SOUND_FLAG_NO_PITCH | MA_SOUND_FLAG_NO_SPATIALIZATION,
                NULL, NULL, music) != MA_SUCCESS) {
        main_term(EXIT_FAILURE, "Unable to play music %s.\n", file);
    }

    ma_sound_set_looping(music, isLooping);
    ma_sound_start(music);

    return music;
}

void aud_pauseMusic(ma_sound* music)
{
    ma_sound_stop(music);
}

void aud_continueMusic(ma_sound* music)
{
    ma_sound_start(music);
}

void aud_stopMusic(ma_sound* music)
{
    ma_sound_stop(music);
    aud_unloadSound(music);
}
