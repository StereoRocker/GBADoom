// Copyright 2021 Dominic Houghton. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Test application for FBConsole using I_Framebuffer interface

#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "ili9341.hpp"

// ILI9341 pin definitions:
// We are going to use SPI 0, and allocate it to the following GPIO pins.
// Pins can be changed, see the GPIO function select table in the datasheet
// for information on GPIO assignments.
#define SPI_PORT spi0
#define PIN_MISO 4
#define PIN_SCK  2
#define PIN_MOSI 3
#define PIN_CS   5
#define PIN_DC   6
#define PIN_RST  7

ILI9341* display;

uint8_t* frame;

extern "C" void c_main(uint8_t* fb);

int main()
{

    // Set all pins high, to avoid chip selecting an incorrect device
    for (int pin = 0; pin < 29; pin++)
    {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_OUT);
        gpio_put(pin, 1);
    }
    
    // Initialise all SDK-provided stdio drivers
    stdio_init_all();

    // Initialise ILI9341 display
    display = new ILI9341(SPI_PORT, PIN_MISO, PIN_MOSI, PIN_SCK,
                            PIN_CS, PIN_DC, PIN_RST,
                            240, 320, 0,
                            62.5 * 1000 * 1000);

    // Set up static framebuffer
    frame = (uint8_t*)malloc(320*240);
    memset(frame, 0, 320*240);

    c_main(frame);
    

    // Break, and halt execution
    __breakpoint();
    for(;;);
    return 0;
}
