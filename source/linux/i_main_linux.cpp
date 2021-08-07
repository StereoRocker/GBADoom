// Copyright 2021 Dominic Houghton. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#if LINUX == 1

#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// Audio
extern "C" {
#include "sounds.h"
#include "w_wad.h"
}
#include "pwm_audio.h"

extern "C" {
volatile soundChannel_t soundChannels[MAX_CHANNELS];
}

// SDL
#include <SDL.h>

/*
 * PWM Interrupt Handler which outputs PWM level and advances the 
 * current sample. 
 * 
 * We repeat the same value for 8 cycles this means sample rate etc
 * adjust by factor of 8   (this is what bitshifting <<3 is doing)
 * 
 */
void AudioCallback(void* userdata, Uint8* stream, int len) {
    while (len--) {
        uint16_t val = 0;

        // Now, for each channel we need to copy sample data.
        for (int ch = 0; ch < MAX_CHANNELS; ch++)
        {
            // channel active?
            if (soundChannels[ch].lump != NULL)
            {
                if (soundChannels[ch].offset >= (soundChannels[ch].size))
                {
                    soundChannels[ch].volume = 0;
                    soundChannels[ch].lump = NULL;
                    continue;
                }
                val += (soundChannels[ch].lump[soundChannels[ch].offset]);
                soundChannels[ch].offset++;
            }
        }

        *stream = (uint8_t)(val >> 3);
        stream++;
    }
}

/**
 * Immediately shuts off sound
 */
extern "C" void muteSound()
{
    for (int ch = 0; ch < MAX_CHANNELS; ch++)
    {
        soundChannels[ch].volume = 0;
        soundChannels[ch].lump = NULL;
    }
}



// SDL vars
SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;
SDL_Surface* renderSurface = NULL;
const int SCREEN_SCALE = 2;
const int SCREEN_WIDTH = 240 * SCREEN_SCALE;
const int SCREEN_HEIGHT = 160 * SCREEN_SCALE;


// Currently unable to go larger - E1M3 fails to load (ZMalloc fails to allocate 504 bytes) with 320x240px buffer
uint8_t frame[240*160];
uint16_t frame_palette[256];
uint16_t real_frame[240*160];

extern "C" void c_main(uint8_t* fb);

void render_fb(void)
{
    // Convert the 8-bit frame to 16-bit colour

    // For each line
    for (int i = 0; i < 160; i++)
    {
        // For each pixel on the line
        for (int j = 0; j < 240; j++)
        {
            real_frame[(i * 240) + j] = frame_palette[frame[(i*240) + j]];
        }
    }

    // Tell SDL to render and display the frame
    SDL_Rect dest;
    dest.x = dest.y = 0;
    dest.w = SCREEN_WIDTH;
    dest.h = SCREEN_HEIGHT;
    SDL_BlitScaled(renderSurface, NULL, screenSurface, &dest);
    SDL_UpdateWindowSurface(window);
}

int main()
{
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        exit(1);
    }

    window = SDL_CreateWindow("Picodoom - Debug", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        SDL_Quit();
        exit(1);
    }

    screenSurface = SDL_GetWindowSurface(window);
    renderSurface = SDL_CreateRGBSurfaceWithFormatFrom(real_frame, 240, 160, 2, 480, SDL_PIXELFORMAT_RGB565);

    // Init audio
    SDL_AudioDeviceID adid;
    SDL_AudioSpec want, have;

    memset(&want, 0, sizeof(want));
    want.freq = 11025;
    want.format = AUDIO_U8;
    want.channels = 1;
    want.samples = 2048;
    want.callback = AudioCallback;

    adid = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (adid == 0)
    {
        printf("Failed to open audio: %s\n", SDL_GetError() );
    } else {
        SDL_PauseAudioDevice(adid, 0);
    }
    
    //memset(audioBuffer, 0, AUDIO_BUF_LEN);    // Clear buffer
    memset((void*)soundChannels, 0, sizeof(soundChannels));

    // Set up static framebuffer
    memset(frame, 0, 240*160);

    // Set up controls
    //shift_init();

    c_main(frame);
    

    // Return from execution - not sure how this can ever happen, actually
    //__breakpoint();
    return 0;
}

#endif