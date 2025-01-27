/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "aud.h"
#include "main.h"

// Variables
ma_engine engine;

// Function implementations

void aud_init(float vol) {
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
	main_term(EXIT_FAILURE, "Failed to initialise audio engine.\n");
    }
    ma_engine_set_volume(&engine, vol);
}

void aud_term(void) {
    ma_engine_uninit(&engine);
}

// Preload sound
// https://github.com/mackron/miniaudio/issues/249
ma_sound* aud_sound_load(const char* file) {
    ma_sound* sound = (ma_sound*) malloc(sizeof(ma_sound));

    // Load and decode now to avoid overhead during game play
    if (ma_sound_init_from_file(&engine, file,
	    MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_PITCH | MA_SOUND_FLAG_NO_SPATIALIZATION,
	    NULL, NULL, sound) != MA_SUCCESS) {
	main_term(EXIT_FAILURE, "Unable to load sound %s.\n", file);
    }

    return sound;
}

void aud_sound_unload(ma_sound *sound) {
    ma_sound_uninit(sound);
    free(sound);
}

// Will use cached sound data and not actually load sound again
ma_sound* aud_sound_play(const char* file) {
    ma_sound* sound = aud_sound_load(file);
    ma_sound_start(sound);
    return sound;
}

void aud_sound_stop(ma_sound* sound) {
    ma_sound_stop(sound);
}
