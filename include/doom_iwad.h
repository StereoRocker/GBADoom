#ifndef DOOM_IWAD_H
#define DOOM_IWAD_H

#if PICO == 1

extern const unsigned char* doom_iwad;
extern const unsigned int doom_iwad_len;

#endif

#if LINUX == 1

extern unsigned char* doom_iwad;
extern unsigned int doom_iwad_len;

#endif

#endif // DOOM_IWAD_H
