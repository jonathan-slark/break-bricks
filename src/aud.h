/*
 * See aud.c for details.
 */

#include <miniaudio.h>

void      aud_init(float vol);
void      aud_term(void);

ma_sound* aud_soundLoad(const char* file);
void      aud_soundUnload(ma_sound* sound);
ma_sound* aud_soundPlay(const char* file);
void      aud_soundStart(ma_sound* sound);
void      aud_soundStop(ma_sound* sound);
