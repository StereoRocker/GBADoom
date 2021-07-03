/* DOOM lookup table generator for picodoom
 * 
 * Uses code from Chocolate Doom, which is available under the following
 * license:
 * 
 * Copyright(C) 1993-1996 Id Software, Inc.
 * Copyright(C) 2005-2014 Simon Howard
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <iostream>
#include <fstream>
#include <limits.h>
#include <string.h>

#include "tables.h"

/* Some DOOM constants */
#define FRACBITS 16
#define FRACUNIT (1 << FRACBITS)
#define ANGLETOFINESHIFT 19
#define FIELDOFVIEW 2048

/* Some DOOM functions */
inline fixed_t FixedMul(fixed_t a, fixed_t b)
{
    return (fixed_t)((int64_t) a*b >> FRACBITS);
}

inline static int D_abs(fixed_t x)
{
  fixed_t _t = (x),_s;
  _s = _t >> (8*sizeof _t-1);
  return (_t^_s)-_s;
}

inline static fixed_t FixedDiv(fixed_t a, fixed_t b)
{
    return ((unsigned)D_abs(a)>>14) >= (unsigned)D_abs(b) ? ((a^b)>>31) ^ INT32_MAX :
                                                  (fixed_t)(((int64_t) a << FRACBITS) / b);
}

/* detailshift = 0 (high res), or 1 (low res) */
#define detailshift 0

int main(int argc, char* argv[])
{
    // Check the number of parameters
    if (argc < 3)
    {
        // Output usage string
        std::cerr << "Usage: " << argv[0] << " <SCREENWIDTH> <SCREENHEIGHT> [ST_HEIGHT]" << std::endl;
        std::cerr << "Outputs a file named tables_SCREENWIDTHxSCREENHEIGHT.h in current working directory" << std::endl;
        return 1;
    }

    // Try to parse the arguments. They'll thrown an exception if they fail.
    int32_t SCREENWIDTH = std::stoi(std::string(argv[1]));
    int32_t SCREENHEIGHT = std::stoi(std::string(argv[2]));
    int32_t ST_HEIGHT = 32;
    /*if (argc > 3)
        ST_HEIGHT = std::stoi(std::string(argv[3]));*/

    // Check that screen dimensions are sane (enough)
    if ((SCREENWIDTH) < 2 || (SCREENHEIGHT < 2))
    {
        std::cerr << "SCREENWIDTH and SCREENHEIGHT must be more than 2 pixels." << std::endl;
    }

    int viewheight = SCREENHEIGHT - ST_HEIGHT;
    int viewwidth = SCREENWIDTH >> detailshift;

    // Start generating tables
    fixed_t cosadj, dy, focallength;
    int x, i, t;
    int centerx = viewwidth /2;
    int centerxfrac = centerx << FRACBITS;

    // Declare and calculate viewangletox
    focallength = FixedDiv (centerxfrac, finetangent[FINEANGLES/4+FIELDOFVIEW/2] );
	int32_t viewangletox[FINEANGLES/2];
    for (i=0 ; i<FINEANGLES/2 ; i++)
    {
        if (finetangent[i] > FRACUNIT*2)
            t = -1;
        else if (finetangent[i] < -FRACUNIT*2)
            t = viewwidth+1;
        else
        {
            t = FixedMul (finetangent[i], focallength);
            t = (centerxfrac - t+FRACUNIT-1)>>FRACBITS;

            if (t < -1)
            t = -1;
            else if (t>viewwidth+1)
            t = viewwidth+1;
        }
        viewangletox[i] = t;
    }

    // Declare and calculate xtoviewangle
    angle_t xtoviewangle[SCREENWIDTH+1];
    for (x=0;x<=viewwidth;x++)
    {
        i = 0;
        while (viewangletox[i]>x)
            i++;
        xtoviewangle[x] = (i<<ANGLETOFINESHIFT)-ANG90;
    }

    // Take out the fencepost cases from viewangletox.
    for (i=0 ; i<FINEANGLES/2 ; i++)
    {
        t = FixedMul (finetangent[i], focallength);
        t = centerx - t;

        if (viewangletox[i] == -1)
            viewangletox[i] = 0;
        else if (viewangletox[i] == viewwidth+1)
            viewangletox[i]  = viewwidth;
    }

    // Declare and calculate yslope
    fixed_t yslope[SCREENHEIGHT];
    memset(yslope, SCREENHEIGHT*sizeof(fixed_t), 0);
    for (i=0 ; i<viewheight ; i++)
    {
        dy = ((i-viewheight/2)<<FRACBITS)+FRACUNIT/2;
        dy = abs(dy);
        yslope[i] = FixedDiv ( (viewwidth<<detailshift)/2*FRACUNIT, dy);
    }

    // Declare and calculate distscale
    fixed_t distscale[SCREENWIDTH];
    for (i=0 ; i<viewwidth ; i++)
    {
        cosadj = abs(finecosine[xtoviewangle[i]>>ANGLETOFINESHIFT]);
        distscale[i] = FixedDiv (FRACUNIT,cosadj);
    }

    // Open an output file
    std::string filename = "tables_" + std::string(argv[1]) + "x" + std::string(argv[2]) + ".h";
    std::ofstream f;
    f.open(filename);

    // Output header guard
    f << "#ifndef " << filename << std::endl;
    f << "#define " << filename << std::endl;
    f << std::endl;

    // Output viewangletox
    f << "const int viewangletox[" << (FINEANGLES/2) << "] = {" << std::endl;
    f << "\t";
    for (i = 0; i < FINEANGLES/2; i++)
    {
        f << viewangletox[i] << ", ";
        if (i % 16 == 15)
            f << std::endl << "\t";
    }
    f << "};" << std::endl << std::endl;

    // Output xtoviewangle
    f << "const angle_t xtoviewangle[" << (SCREENWIDTH+1) << "] = {" << std::endl;
    f << "\t";
    for (i = 0; i < SCREENWIDTH+1; i++)
    {
        f << xtoviewangle[i] << ", ";
        if (i % 16 == 15)
            f << std::endl << "\t";
    }
    f << "};" << std::endl << std::endl;

    // Output yslope
    f << "const fixed_t yslope[" << (SCREENHEIGHT) << "] = {" << std::endl;
    f << "\t";
    for (i = 0; i < SCREENHEIGHT; i++)
    {
        f << yslope[i] << ", ";
        if (i % 16 == 15)
            f << std::endl << "\t";
    }
    f << "};" << std::endl << std::endl;

    // Output distscale
    f << "const fixed_t distscale[" << (SCREENWIDTH) << "] = {" << std::endl;
    f << "\t";
    for (i = 0; i < SCREENWIDTH; i++)
    {
        f << distscale[i] << ", ";
        if (i % 16 == 15)
            f << std::endl << "\t";
    }
    f << "};" << std::endl << std::endl;

    // Output header guard end
    f << std::endl;
    f << "#endif" << std::endl;

    // Flush output and close
    f.flush();
    f.close();

    std::cout << "Successfully wrote tables to " << filename << std::endl;

    return 0;
}