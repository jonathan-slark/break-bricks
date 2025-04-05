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
    sounds[SoundBrick] = aud_soundLoad(FILE_BRICK);
    sounds[SoundDeath] = aud_soundLoad(FILE_DEATH);
    sounds[SoundClear] = aud_soundLoad(FILE_CLEAR);
    sounds[SoundWon]   = aud_soundLoad(FILE_WON);
    sounds[SoundLost]  = aud_soundLoad(FILE_LOST);

    music = (ma_sound **) malloc(COUNT(MUSIC) * sizeof(ma_sound *));
    for (size_t i = 0; i < COUNT(MUSIC); i++)
    {
        music[i] = aud_soundLoad(MUSIC[i]);
    }
}

void audio_unload(void)
{
    if (music)
    {
	for (size_t i = 0; i < COUNT(MUSIC); i++)
	{
	    if (music[i]) aud_soundUnload(music[i]);
	}
	free(music);
    }

    if (sounds)
    {
        for (Sound i = 0; i < SoundCount; i++)
        {
            if (sounds[i]) aud_soundUnload(sounds[i]);
        }
        free(sounds);
    }

    aud_term();
}

void audio_playMusic(int level)
{
    playing = aud_soundPlay(MUSIC[level - 1]);
}

void audio_stopMusic(void)
{
    if (playing)
    {
	aud_soundStop(playing);
	playing = nullptr;
    }
}
