/*
 * See aud.c for details.
 */

#include <miniaudio.h>

void      aud_init(float vol);
void      aud_term(void);

ma_sound* aud_loadSound(const char* file, bool isLooping);
void      aud_unloadSound(ma_sound* sound);
ma_sound* aud_playSound(const char* file, bool isLooping);
void      aud_pauseSound(ma_sound* sound);
void      aud_continueSound(ma_sound* sound);
void      aud_stopSound(ma_sound* sound);
