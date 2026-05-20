// src/audio/audio.c - Fixed for PS Vita
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "audio.h"

#define SAMPLE_RATE 22050  // Lower sample rate for Vita compatibility
#define AMPLITUDE 16000    // Lower amplitude to avoid clipping
#define NUM_VOICES 5

typedef enum {
    WAVE_SINE = 0,
    WAVE_SQUARE,
    WAVE_TRIANGLE,
    WAVE_SAW
} WaveType;

typedef struct {
    int note;
    int instrument;
    WaveType wave;
    float phase;
    float phase_step;
    int volume;
    int active;
} Voice;

static Voice voices[NUM_VOICES];
static SDL_AudioDeviceID audio_device = 0;
static int audio_initialized = 0;

// Simpler frequency table
static float get_frequency(int note) {
    if (note < 1 || note > 72) return 0.0f;
    // A4 (note 34) = 440Hz
    return 440.0f * powf(2.0f, (note - 34) / 12.0f);
}

// Audio callback - this runs in a separate thread
static void audio_callback(void *userdata, Uint8 *stream, int len) {
    Sint16 *buffer = (Sint16 *)stream;
    int samples = len / sizeof(Sint16);
    
    // Fill buffer with silence first
    memset(buffer, 0, len);
    
    for (int i = 0; i < samples; i++) {
        float mixed = 0.0f;
        int active_voices = 0;
        
        for (int v = 0; v < NUM_VOICES; v++) {
            if (!voices[v].active) continue;
            
            active_voices++;
            float val = 0.0f;
            float p = voices[v].phase;
            
            switch (voices[v].wave) {
                case WAVE_SINE:
                    val = sinf(2.0f * M_PI * p);
                    break;
                case WAVE_SQUARE:
                    val = (p < 0.5f) ? 1.0f : -1.0f;
                    break;
                case WAVE_TRIANGLE:
                    if (p < 0.25f) val = 4.0f * p;
                    else if (p < 0.75f) val = 2.0f - 4.0f * p;
                    else val = 4.0f * p - 4.0f;
                    break;
                case WAVE_SAW:
                    val = 2.0f * p - 1.0f;
                    break;
            }
            
            mixed += val * (voices[v].volume / 128.0f) * AMPLITUDE;
            
            // Advance phase
            voices[v].phase += voices[v].phase_step;
            if (voices[v].phase >= 1.0f) voices[v].phase -= 1.0f;
            if (voices[v].phase < 0.0f) voices[v].phase += 1.0f;
        }
        
        // Soft clipping
        if (mixed > AMPLITUDE) mixed = AMPLITUDE;
        if (mixed < -AMPLITUDE) mixed = -AMPLITUDE;
        
        buffer[i] = (Sint16)mixed;
    }
}

int audio_init() {
    if (audio_initialized) {
        printf("Audio already initialized\n");
        return 0;
    }
    
    printf("Initializing audio system...\n");
    
    // Initialize all voices
    for (int i = 0; i < NUM_VOICES; i++) {
        voices[i].active = 0;
        voices[i].note = 0;
        voices[i].wave = WAVE_SINE;
        voices[i].phase = 0.0f;
        voices[i].phase_step = 0.0f;
        voices[i].volume = 100;
        voices[i].instrument = 1;
    }
    
    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = SAMPLE_RATE;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 512;  // Smaller buffer for lower latency
    want.callback = audio_callback;
    want.userdata = NULL;
    
    printf("Opening audio device: %d Hz, %d channels, %d samples\n", 
           want.freq, want.channels, want.samples);
    
    audio_device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    
    if (audio_device == 0) {
        printf("ERROR: Failed to open audio device: %s\n", SDL_GetError());
        
        // Try with default settings
        printf("Trying with NULL desired spec...\n");
        audio_device = SDL_OpenAudioDevice(NULL, 0, NULL, &have, SDL_AUDIO_ALLOW_ANY_CHANGE);
        if (audio_device == 0) {
            printf("ERROR: Failed to open audio with default settings: %s\n", SDL_GetError());
            return -1;
        }
    }
    
    printf("Audio device opened successfully!\n");
    printf("  Frequency: %d\n", have.freq);
    printf("  Format: %d\n", have.format);
    printf("  Channels: %d\n", have.channels);
    printf("  Samples: %d\n", have.samples);
    
    // Start playback
    SDL_PauseAudioDevice(audio_device, 0);
    
    audio_initialized = 1;
    printf("Audio playback started. %d voices ready.\n", NUM_VOICES);
    
    return 0;
}

void audio_shutdown() {
    if (audio_initialized && audio_device != 0) {
        printf("Shutting down audio...\n");
        SDL_PauseAudioDevice(audio_device, 1);
        SDL_CloseAudioDevice(audio_device);
        audio_device = 0;
        audio_initialized = 0;
        printf("Audio shut down.\n");
    }
}

void audio_note_on(int channel, int note, int instrument, int effect) {
    if (!audio_initialized) {
        printf("audio_note_on: Audio not initialized!\n");
        return;
    }
    
    if (channel < 0 || channel >= NUM_VOICES) {
        printf("audio_note_on: Invalid channel %d\n", channel);
        return;
    }
    
    if (note < 1 || note > 72) {
        printf("audio_note_on: Invalid note %d\n", note);
        return;
    }
    
    Voice *v = &voices[channel];
    v->note = note;
    v->instrument = instrument;
    v->active = 1;
    v->phase = 0.0f;
    
    float freq = get_frequency(note);
    v->phase_step = freq / SAMPLE_RATE;
    
    // Instrument selects waveform
    switch (instrument) {
        case 1: v->wave = WAVE_SINE;   v->volume = 120; break;
        case 2: v->wave = WAVE_SQUARE; v->volume = 100; break;
        case 3: v->wave = WAVE_TRIANGLE; v->volume = 110; break;
        case 4: v->wave = WAVE_SAW;    v->volume = 90; break;
        case 5: v->wave = WAVE_SQUARE; v->volume = 70; break;  // Drums
        default: v->wave = WAVE_SINE;  v->volume = 100; break;
    }
    
    // Handle effect
    if (effect != 0) {
        int fx_type = (effect >> 8) & 0xFF;
        int fx_val = effect & 0xFF;
        switch (fx_type) {
            case 0x0C: // Cxx - Volume
                v->volume = fx_val;
                printf("  Volume effect: %d\n", fx_val);
                break;
        }
    }
    
    printf("NOTE ON: ch=%d note=%d inst=%d freq=%.1fHz wave=%d vol=%d\n", 
           channel, note, instrument, freq, v->wave, v->volume);
}

void audio_note_off(int channel) {
    if (!audio_initialized) return;
    if (channel < 0 || channel >= NUM_VOICES) return;
    
    voices[channel].active = 0;
    voices[channel].note = 0;
    printf("NOTE OFF: ch=%d\n", channel);
}
