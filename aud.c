/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "main.h"
#include "aud.h"

// Variables
ma_engine engine;

// Function implementations

void aud_init(float vol) {
    if (ma_engine_init(NULL, &engine) != MA_SUCCESS) {
	main_term(EXIT_FAILURE, "Failed to initialise audio engine.\n");
    }
    ma_engine_set_volume(&engine, vol);
}

ma_sound* aud_sound_load(const char* file) {
    ma_sound* sound = (ma_sound*) malloc(sizeof(ma_sound));

    // Load and decode now to avoid overhead during game play
    if (ma_sound_init_from_file(&engine, file, MA_SOUND_FLAG_DECODE, NULL,
		NULL, sound) != MA_SUCCESS) {
	main_term(EXIT_FAILURE, "Unable to load sound %s.\n", file);
    }

    return sound;
}

void aud_sound_unload(ma_sound *sound) {
    ma_sound_uninit(sound);
    free(sound);
}

void aud_playsound(ma_sound *sound) {
    if (ma_sound_is_playing(sound)) {
	ma_sound_stop(sound);
	ma_sound_seek_to_pcm_frame(sound, 0);
    }
    ma_sound_start(sound);
}

void aud_term(void) {
    ma_engine_uninit(&engine);
}
