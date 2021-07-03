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

#include "hardware/adc.h"

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

uint8_t frame[320*240];
uint16_t frame_palette[256];

extern "C" void c_main(uint8_t* fb);

void render_fb(void)
{
    // Here's hoping the stack has 480 bytes free for me lmao
    uint16_t line[240];

    // For each line
    for (int i = 0; i < 160; i++)
    {
        // For each pixel on the line
        for (int j = 0; j < 240; j++)
        {
            line[j] = frame_palette[frame[(i*240) + j]];
        }

        // Output the line to display
        display->plot_block(0, i, 239, i+1, line, 240);
    }
}

#define ADC_AX   0
#define PIN_AX   26

#define ADC_AY   1
#define PIN_AY   27

#define PIN_SEL  28
#define PIN_FIRE 29

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
                            320, 240, 90,
                            62.5 * 1000 * 1000);

    // Set up static framebuffer
    memset(frame, 0, 240*160);

    // Set up controls
    adc_init();
    adc_gpio_init(PIN_AX);
    adc_gpio_init(PIN_AY);

    gpio_init(PIN_SEL);
    gpio_pull_up(PIN_SEL);
    gpio_set_dir(PIN_SEL, GPIO_IN);

    gpio_init(PIN_FIRE);
    gpio_pull_up(PIN_FIRE);
    gpio_set_dir(PIN_FIRE, GPIO_IN);

    c_main(frame);
    

    // Break, and halt execution
    __breakpoint();
    for(;;);
    return 0;
}
