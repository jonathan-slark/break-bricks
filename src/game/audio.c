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
    "music/HoliznaCC0_-_2nd_Dimension.mp3", // One track per level
    "music/HoliznaCC0_-_Astroids.mp3",
    "music/HoliznaCC0_-_In_The_End.mp3",
    "music/HoliznaCC0_-_Pixel_Party.mp3",
    "music/HoliznaCC0_-_Scroller.mp3",
    "music/HoliznaCC0_-_Space_Castle.mp3",
    "music/HoliznaCC0_-_Where_Its_Safe.mp3"
};

// Variables
static ma_sound **sounds = nullptr;
static ma_sound *playing = nullptr;

// Function definitions

void audio_load(void)
{
    aud_init(VOL);

    sounds = (ma_sound **) malloc(SoundCount * sizeof(ma_sound *));
    for (Sound s = 0; s < SoundCount; s++) {
	sounds[s] = aud_loadSound(SOUNDS[s], false);
    }
}

void audio_unload(void)
{
    if (sounds) {
        for (Sound i = 0; i < SoundCount; i++) {
            if (sounds[i]) aud_unloadSound(sounds[i]);
        }
        free(sounds);
    }
    audio_stopMusic();

    aud_term();
}

void audio_playMusic(int level)
{
    if (playing != nullptr) aud_stopMusic(playing);
    playing = aud_playMusic(MUSIC[level], true);
}

void audio_pauseMusic(void)
{
    if (playing != nullptr) {
	aud_pauseMusic(playing);
    }
}

void audio_stopMusic(void)
{
    if (playing != nullptr) {
	aud_stopMusic(playing);
	playing = nullptr;
    }
}

void audio_continueMusic(void)
{
    if (playing != nullptr) {
	aud_continueMusic(playing);
    }
}

void audio_playSound(Sound s)
{
    aud_playSound(SOUNDS[s]);
}
