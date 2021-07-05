// Copyright 2021 Dominic Houghton. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

/* Frame-buffer abstraction interface

 * Written as a template, to allow for different pixel packing.
 * 
 * get_color is expected to take RGB values 0-255, and return data as it would
 * expect to receive in an array, as per the plot_block function.
 * 
 * plot_block is expected to take an array of data, where pixel data is indexed
 * with the following formula:
 * index = (width * y) + x
 * 
 * scroll_vertical is expected to shift the contents of the display a number of
 * pixels. This can be by copying and rewriting the framebuffer, or by using
 * addressing modes provided by the display driver and keeping an offset. See
 * StereoRocker's implementation of the ili9341 framebuffer for an example of
 * using addressing modes provided by the chip.
 */

#ifndef I_FRAMEBUFFER_H
#define I_FRAMEBUFFER_H

#include <stdint.h>

template<class T>
class I_Framebuffer {
    public:
        virtual T get_color(uint8_t r, uint8_t g, uint8_t b);
        
        virtual void get_dimensions(uint16_t* width, uint16_t* height);

        virtual void plot_block(uint16_t x0, uint16_t y0,
                                uint16_t x1, uint16_t y1,
                                T* pixeldata, uint32_t len);
        
        virtual void scroll_vertical(uint16_t pixels);
};

#endif