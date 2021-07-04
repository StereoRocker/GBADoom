#pragma GCC optimize ("-O0")

//#include "iwad/doom1.c"
//#include "iwad/doomu.c"
//#include "iwad/doom2.c"
//#include "iwad/tnt.c"
//#include "iwad/plutonia.c"
//#include "iwad/sigil.c"

#include "pico/stdlib.h"

#include "doom_iwad.h"

// IWAD data is present at 2MiB mark in flash
const unsigned char* doom_iwad = (const unsigned char*)(XIP_BASE + (2 * 1024 * 1024));

//const unsigned int doom_iwad_len = 3842044UL;
const unsigned int doom_iwad_len = 4324136UL;

