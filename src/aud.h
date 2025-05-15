/*
 * See aud.c for details.
 */

#pragma once

#include <miniaudio.h>

// Function prototypes
void      aud_init(float vol);
void      aud_term(void);
ma_sound* aud_loadSound(const char* file, bool isLooping);
void      aud_unloadSound(ma_sound* sound);
void      aud_playSound(const char* file);
ma_sound* aud_playMusic(const char* file, bool isLooping);
void      aud_pauseMusic(ma_sound* music);
void      aud_continueMusic(ma_sound* music);
void      aud_stopMusic(ma_sound* music);
