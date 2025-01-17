/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

void      aud_init(float vol);
void      aud_term(void);

ma_sound* aud_sound_load(const char* file, bool islooping);
void      aud_sound_unload(ma_sound *sound);
void      aud_sound_play(const char* file);
void      aud_sound_start(ma_sound* sound);
