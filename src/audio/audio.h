// src/audio/audio.h
#ifndef AUDIO_H
#define AUDIO_H

int audio_init();
void audio_shutdown();
void audio_note_on(int channel, int note, int instrument, int effect);
void audio_note_off(int channel);

#endif
