#pragma once

// Types
typedef enum
{
    SoundBrick,
    SoundDeath,
    SoundClear,
    SoundWon,
    SoundLost,
    SoundCount
} Sound;

// Function prototypes
void audio_load(void);
void audio_unload(void);
void audio_playMusic(int level);
void audio_stopMusic(void);
