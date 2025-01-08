/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "main.h"
#include "aud.h"

// Function declarations
void datacallback(ma_device* dev, void* output, const void* input, ma_uint32 framecount);

// Constants
static const unsigned FORMAT      = ma_format_f32;
static const unsigned CHANNELS    = 2;
static const unsigned SAMPLE_RATE = 48000;
// Variables
ma_device device;

// Function implementations

void datacallback(ma_device* dev, void* output, const void* input, ma_uint32 framecount) {
}

void aud_init(void) {
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = FORMAT;
    config.playback.channels = CHANNELS;
    config.sampleRate        = SAMPLE_RATE;
    config.dataCallback      = datacallback;

    if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
        main_term(EXIT_FAILURE, "Unable to initialise audio.\n");
    }

    ma_device_start(&device);
}

void aud_term(void) {
    ma_device_uninit(&device);
}
