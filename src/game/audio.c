#include <stdlib.h> // malloc

#include "../aud.h"
#include "../util.h"
#include "audio.h"

// Constants
static const float VOL          = 0.1; // Volume 0 - 1
static const char  FILE_BRICK[] = "sfx/brick.wav";
static const char  FILE_DEATH[] = "sfx/death.wav";
static const char  FILE_CLEAR[] = "sfx/clear.wav";
static const char  FILE_WON[]   = "sfx/won.wav";
static const char  FILE_LOST[]  = "sfx/lost.wav";
static const char* SOUNDS[SoundCount] =
{
    FILE_BRICK,
    FILE_DEATH,
    FILE_CLEAR,
    FILE_WON,
    FILE_LOST
};
static const char* const MUSIC[] =
{
    "music/HoliznaCC0 - 2nd Dimension.mp3", // One track per level
    "music/HoliznaCC0 - Astroids.mp3",
    "music/HoliznaCC0 - In The End.mp3",
    "music/HoliznaCC0 - Pixel Party.mp3",
    "music/HoliznaCC0 - Scroller.mp3",
    "music/HoliznaCC0 - Space Castle.mp3",
    "music/HoliznaCC0 - Where It's Safe.mp3"
};

// Variables
static ma_sound **sounds;
static ma_sound **music;
static ma_sound *playing;

// Function definitions

void audio_load(void)
{
    aud_init(VOL);

    sounds = (ma_sound **) malloc(SoundCount * sizeof(ma_sound *));
    for (Sound s = 0; s < SoundCount; s++) {
	sounds[s] = aud_loadSound(SOUNDS[s], false);
    }

    music = (ma_sound **) malloc(COUNT(MUSIC) * sizeof(ma_sound *));
    for (size_t i = 0; i < COUNT(MUSIC); i++) {
        music[i] = aud_loadSound(MUSIC[i], true);
    }
}

void audio_unload(void)
{
    if (music) {
	for (size_t i = 0; i < COUNT(MUSIC); i++) {
	    if (music[i]) aud_unloadSound(music[i]);
	}
	free(music);
    }

    if (sounds) {
        for (Sound i = 0; i < SoundCount; i++) {
            if (sounds[i]) aud_unloadSound(sounds[i]);
        }
        free(sounds);
    }

    aud_term();
}

void audio_playMusic(int level)
{
    playing = aud_playSound(MUSIC[level], true);
}

void audio_pauseMusic(void)
{
    if (playing) {
	aud_pauseSound(playing);
    }
}

void audio_stopMusic(void)
{
    if (playing) {
	aud_stopSound(playing);
	playing = nullptr;
    }
}

void audio_continueMusic(void)
{
    if (playing) {
	aud_continueSound(playing);
    }
}

void audio_playSound(Sound s)
{
    aud_playSound(SOUNDS[s], false);
}
