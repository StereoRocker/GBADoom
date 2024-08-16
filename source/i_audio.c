/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom: a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2000 by
 *  Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze
 *  Copyright 2005, 2006 by
 *  Florian Schulze, Colin Phipps, Neil Stevens, Andrey Budko
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 *  02111-1307, USA.
 *
 * DESCRIPTION:
 *  System interface for sound.
 *
 *-----------------------------------------------------------------------------
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if LINUX == 1
#include <stdint.h>
#include <ctype.h>

void strupr(char* str)
{
    while (*str)
    {
        *str = toupper(*str);
        str++;
    }
}

#endif

#include <math.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "z_zone.h"

#include "m_swap.h"
#include "i_sound.h"
#include "m_misc.h"
#include "w_wad.h"
#include "lprintf.h"
#include "s_sound.h"

#include "doomdef.h"
#include "doomstat.h"
#include "doomtype.h"

#include "d_main.h"

#include "m_fixed.h"

#include "global_data.h"
#include "pwm_audio.h"
#include "doom_iwad.h"

//
// Starting a sound means adding it
//  to the current list of active sounds
//  in the internal channels.
// As the SFX info struct contains
//  e.g. a pointer to the raw data,
//  it is ignored.
// As our sound handling does not handle
//  priority, it is ignored.
// Pitching (that is, increased speed of playback)
//  is set, but currently not used by mixing.
//
int I_StartSound(int id, int channel, int vol, int sep)
{
	if ((channel < 0) || (channel >= MAX_CHANNELS))
		return -1;

    // Find the sound first
    char soundbuf[9];
    snprintf(soundbuf, 9, "ds%s", S_sfx[id].name);
    strupr(soundbuf);

    // get lump
    int lumpNum = W_CheckNumForName(soundbuf);
    if (lumpNum == -1)
    {
        printf("I_StartSound: Failed to find %s\n", soundbuf);
        return -1;
    }
    const void *lumpPtr = W_CacheLumpNum(lumpNum);
    // get sample size
    int32_t size = W_LumpLength(lumpNum);


    soundChannels[channel].offset = 0;
    soundChannels[channel].size = size;
    soundChannels[channel].volume = vol;
    soundChannels[channel].lastAudioBufferIdx = 0xFFFF;
    soundChannels[channel].lump = (uint8_t*)lumpPtr;

    

	return channel;
}

void I_ShutdownSound(void)
{
    if (_g->sound_inited)
	{
		lprintf(LO_INFO, "I_ShutdownSound: ");
		lprintf(LO_INFO, "\n");
        _g->sound_inited = false;
	}
}

//static SDL_AudioSpec audio;

void I_InitSound(void)
{

//#ifdef __arm__
#if 0
    mmInitDefault(soundbank_bin, 12);
#endif

	if (!nomusicparm)
		I_InitMusic();

	// Finished initialization.
    lprintf(LO_INFO,"I_InitSound: sound ready");
}

void I_InitMusic(void)
{
}

void I_PlaySong(int handle, int looping)
{
    if(handle == mus_None)
        return;

//#ifdef __arm__
#if 0
    mm_pmode mode = looping ? MM_PLAY_LOOP : MM_PLAY_ONCE;

    unsigned int song = musicMap[handle].mm_num;

    mmStart(song, mode);
#endif
}


void I_PauseSong (int handle)
{
#ifdef __arm__
    //mmPause();
#endif
}

void I_ResumeSong (int handle)
{
#ifdef __arm__
    //mmResume();
#endif
}

void I_StopSong(int handle)
{
#ifdef __arm__
    //mmStop();
#endif
}

void I_SetMusicVolume(int volume)
{
#ifdef __arm__
    //int mmvol = volume * 32;

    //mmSetModuleVolume(mmvol);
#endif
}
