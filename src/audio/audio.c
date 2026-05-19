#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <math.h>
#include <string.h>
#include "audio.h"

#define SAMPLE_RATE 44100
#define AMPLITUDE 28000
#define NUM_VOICES 8 // Should be enough

typedef enum {
    WAVE_SINE = 0,
    WAVE_SQUARE,
    WAVE_TRIANGLE,
    WAVE_SAW
} WaveType;

typedef struct {
    int note;           // MIDI note number (0 = off)
    int instrument;     // 1-16
    WaveType wave;
    float phase;
    float frequency;
    int volume;
    int active;
} Voice;

static Voice voices[NUM_VOICES];
static SDL_AudioDeviceID audio_device = 0;
static int audio_initialized = 0;

// Frequency table for notes 1-72 (C-1 to B-6)
static float note_frequencies[73];

static void init_note_table() {
    for (int i = 1; i <= 72; i++) {
        note_frequencies[i] = 440.0f * powf(2.0f, (i - 34) / 12.0f);
    }
}

// SDL audio callback - generates samples
static void audio_callback(void *userdata, Uint8 *stream, int len) {
    Sint16 *buffer = (Sint16 *)stream;
    int samples = len / 2;  // 16-bit mono
    
    memset(buffer, 0, len);
    
    for (int i = 0; i < samples; i++) {
        float sample = 0.0f;
        int active_count = 0;
        
        for (int v = 0; v < NUM_VOICES; v++) {
            if (!voices[v].active || voices[v].note == 0) continue;
            
            active_count++;
            float freq = note_frequencies[voices[v].note];
            float phase = voices[v].phase;
            float val = 0.0f;
            
            switch (voices[v].wave) {
                case WAVE_SINE:
                    val = sinf(2.0f * M_PI * phase);
                    break;
                case WAVE_SQUARE:
                    val = (sinf(2.0f * M_PI * phase) >= 0) ? 1.0f : -1.0f;
                    break;
                case WAVE_TRIANGLE:
                    val = 2.0f * fabsf(2.0f * (phase - floorf(phase + 0.5f))) - 1.0f;
                    break;
                case WAVE_SAW:
                    val = 2.0f * (phase - floorf(phase + 0.5f));
                    break;
            }
            
            sample += val * (voices[v].volume / 128.0f) * (AMPLITUDE / 4.0f);
            
            // Advance phase
            voices[v].phase += freq / SAMPLE_RATE;
            if (voices[v].phase >= 1.0f) voices[v].phase -= 1.0f;
        }
        
        // Simple soft clipping
        if (sample > AMPLITUDE) sample = AMPLITUDE;
        if (sample < -AMPLITUDE) sample = -AMPLITUDE;
        
        buffer[i] = (Sint16)sample;
    }
}

int audio_init() {
    if (audio_initialized) return 0;
    
    init_note_table();
    
    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = SAMPLE_RATE;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 1024;
    want.callback = audio_callback;
    
    audio_device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (audio_device == 0) {
        printf("Failed to open audio device: %s\n", SDL_GetError());
        return -1;
    }
    
    // Initialize voices
    for (int i = 0; i < NUM_VOICES; i++) {
        voices[i].active = 0;
        voices[i].note = 0;
        voices[i].wave = WAVE_SINE;
        voices[i].volume = 100;
    }
    
    SDL_PauseAudioDevice(audio_device, 0);  // Start playback
    audio_initialized = 1;
    printf("Audio initialized: %d voices ready\n", NUM_VOICES);
    return 0;
}

void audio_shutdown() {
    if (audio_device != 0) {
        SDL_CloseAudioDevice(audio_device);
        audio_device = 0;
    }
    audio_initialized = 0;
}

// Trigger a note on a specific channel
void audio_note_on(int channel, int note, int instrument, int effect) {
  printf("AUDIO NOTE ON\n");
    if (!audio_initialized || channel < 0 || channel >= NUM_VOICES) return;
    if (note < 1 || note > 72) return;
    
    Voice *v = &voices[channel];
    v->note = note;
    v->instrument = instrument;
    v->active = 1;
    v->phase = 0.0f;
    
    // Instrument selects waveform
    switch (instrument) {
        case 1: v->wave = WAVE_SINE; break;
        case 2: v->wave = WAVE_SQUARE; break;
        case 3: v->wave = WAVE_TRIANGLE; break;
        case 4: v->wave = WAVE_SAW; break;
        case 5: v->wave = WAVE_SQUARE; v->volume = 60; break;  // Drums
        default: v->wave = WAVE_SINE; break;
    }
    
    // Handle effect (volume column)
    if (effect != 0) {
        int fx_type = (effect >> 8) & 0xFF;
        int fx_val = effect & 0xFF;
        switch (fx_type) {
            case 0x0C: // Cxx - Volume
                v->volume = fx_val;
                break;
        }
    }
}

// Release a note
void audio_note_off(int channel) {
    if (!audio_initialized || channel < 0 || channel >= NUM_VOICES) return;
    voices[channel].active = 0;
    voices[channel].note = 0;
}
