// Copyright 2021 Dominic Houghton. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// ILI9341 SPI display driver for Raspberry Pico, confirming to I_Framebuffer interface

#include "ili9341.hpp"

#include "pico/stdlib.h"
#include "hardware/spi.h"

#include <stdint.h>
#include <string.h>
#include <array>

ILI9341::ILI9341(spi_inst_t* spiport, uint8_t miso, uint8_t mosi, uint8_t sck, uint8_t cs, uint8_t dc, uint8_t rst, uint16_t width, uint16_t height, uint16_t rotation, uint32_t baudrate) {
    // Store variables passed to constructor
    _SPI = spiport;
    _MISO = miso;
    _MOSI = mosi;
    _SCK = sck;
    _CS = cs;
    _DC = dc;
    _RST = rst;
    _WIDTH = width;
    _HEIGHT = height;
    _SCROLL_OFFSET = 0;

    // Handle rotation
    switch (rotation) {
        case 0:
            _ROTATION = DISPLAY_ROTATE_0;
            break;
        case 90:
            _ROTATION = DISPLAY_ROTATE_90;
            break;
        case 180:
            _ROTATION = DISPLAY_ROTATE_180;
            break;
        case 270:
            _ROTATION = DISPLAY_ROTATE_270;
            break;
        default:
            _ROTATION = DISPLAY_ROTATE_0;
    }

    // Initialise SPI
    int baud = spi_init(_SPI, baudrate);
    printf("SPI Baudrate: %i\n", baud);
    gpio_set_function(_MISO, GPIO_FUNC_SPI);
    gpio_set_function(_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(_MOSI, GPIO_FUNC_SPI);

    // Initialise CS, DC, RST pins
    gpio_init(_CS);
    gpio_set_dir(_CS, GPIO_OUT);
    gpio_put(_CS, 1);

    gpio_init(_RST);
    gpio_set_dir(_RST, GPIO_OUT);
    gpio_put(_RST, 1);

    gpio_init(_DC);
    gpio_set_dir(_DC, GPIO_OUT);
    gpio_put(_DC, 0);

    // Reset display
    reset();
}

void ILI9341::reset()
{
    // Reset pin is active low; pulse it low for 50ms
    gpio_put(_RST, 0);
    sleep_ms(50);
    gpio_put(_RST, 1);
    sleep_ms(50);

    // Re-initialise the display
    initialise();
}

void ILI9341::write_data(uint8_t* args, uint32_t len)
{
    // We're writing data, so drive the DC pin high
    gpio_put(_DC, 1);

    // CS pin is active low, drive it low while writing
    gpio_put(_CS, 0);
    spi_write_blocking(_SPI, args, len);
    gpio_put(_CS, 1);
}

void ILI9341::write_data(uint8_t data)
{
    // We're writing data, so drive the DC pin high
    gpio_put(_DC, 1);

    // CS pin is active low, drive it low while writing
    gpio_put(_CS, 0);
    spi_write_blocking(_SPI, &data, 1);
    gpio_put(_CS, 1);
}

void ILI9341::write_cmd(uint8_t command)
{
    // We're writing a command, so drive the DC pin low
    gpio_put(_DC, 0);

    // CS pin is active low, drive it low while writing
    gpio_put(_CS, 0);
    spi_write_blocking(_SPI, &command, 1);
    gpio_put(_CS, 1);
}

void ILI9341::write_cmd(uint8_t command, uint8_t* data, uint32_t len)
{
    write_cmd(command);
    write_data(data, len);
}

bool ILI9341::bounds(uint16_t x, uint16_t y)
{
    if (x < 0)
        return false;
    if (y < 0)
        return false;
    if (x > _WIDTH)
        return false;
    if (y > _HEIGHT)
        return false;
    
    return true;
}

void ILI9341::plot_block(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t* pixeldata, uint32_t len)
{
    uint8_t locdat[4];

    // Bounds check
    if (!bounds(x0, y0))
        return;
    if (!bounds(x1, y1))
        return;

    // Add offset
    y0 += _SCROLL_OFFSET;
    y1 += _SCROLL_OFFSET;

    // Check if either y0 or y1 is set beyond _HEIGHT, after applying _SCROLL_OFFSET
    if ((y0 >= _HEIGHT) || (y1 >= _HEIGHT))
    {
        // If either y0 or y1 is beyond the framebuffer memory, but the other is not, place two draw calls.
        if (!((y0 >= _HEIGHT) && (y1 >= _HEIGHT)))
        {
            // Calculate the width of the block
            int width = MAX(x0 - x1, x1 - x0) + 1;

            // Calculate the length of the top section of the block
            int first_length = width * (_HEIGHT - y0);

            // Calculate the length of the bottom section of the block
            int second_length = len - first_length;

            // Plot both sections of the block
            plot_block(x0, y0-_SCROLL_OFFSET, x1, _HEIGHT-1-_SCROLL_OFFSET, pixeldata, first_length);
            plot_block(x0, _HEIGHT-_SCROLL_OFFSET, x1, y1-_SCROLL_OFFSET, &(pixeldata[first_length]), second_length);
            
            // Return so we don't try to draw again
            return;
        }

        // Both y0 and y1 are beyond _HEIGHT, after applying _SCROLL_OFFSET, so just account for that and wrap.
        y0 %= _HEIGHT;
        y1 %= _HEIGHT;
    }

    // Prepare the data for SET_COLUMN, expects big endian
    locdat[0] = (x0 >> 8);
    locdat[1] = (x0 & 0xFF);
    locdat[2] = (x1 >> 8);
    locdat[3] = (x1 & 0xFF);
    write_cmd(SET_COLUMN);
    write_data(locdat, 4);

    // Prepare the data for SET_PAGE, expects big endian
    locdat[0] = (y0 >> 8);
    locdat[1] = (y0 & 0xFF);
    locdat[2] = (y1 >> 8);
    locdat[3] = (y1 & 0xFF);
    write_cmd(SET_PAGE);
    write_data(locdat, 4);

    // Write the desired pixel data
    write_cmd(WRITE_RAM);
    write_data((uint8_t*)pixeldata, len*2);
}

void ILI9341::plot_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    plot_block(x, y, x, y, (&color), 1);
}

void ILI9341::clear(uint16_t color)
{
    uint16_t data[_WIDTH * 16];
    std::fill(data, &data[_WIDTH * 16], color);
    

    for (int y = 0; y < _HEIGHT; y+= 8)
    {
        plot_block(0, y, _WIDTH - 1, y + 7, data, _WIDTH * 16);
    }
}

void ILI9341::initialise()
{
    // Software reset
    write_cmd(SWRESET);  
    sleep_ms(100);

    // Pwr ctrl B
    write_cmd(PWCTRB);
    write_data((uint8_t*)PWCTRB_D, 3);

    // Pwr on seq. ctrl
    write_cmd(POSC);
    write_data((uint8_t*)POSC_D, 4);

    // Driver timing ctrl A
    write_cmd(DTCA);
    write_data((uint8_t*)DTCA_D, 3);

    // Pwr ctrl A
    write_cmd(PWCTRA);
    write_data((uint8_t*)PWCTRA_D, 5);


    // Pump ratio control
    write_cmd(PUMPRC);
    write_data(0x20);


    // Driver timing ctrl B
    write_cmd(DTCB);
    write_data((uint8_t*)DTCB_D, 2);


    // Pwr ctrl 1
    write_cmd(PWCTR1);
    write_data(0x23);


    // Pwr ctrl 2
    write_cmd(PWCTR2);
    write_data(0x10);


    // VCOM ctrl 1
    write_cmd(VMCTR1);
    write_data((uint8_t*)VMCTR1_D, 2);


    // VCOM ctrl 2
    write_cmd(VMCTR2);
    write_data(0x86);


    // Memory access ctrl
    write_cmd(MADCTL);
    write_data(_ROTATION);


    // Vertical scrolling start address
    write_cmd(VSCRSADD);
    write_data(0x00);


    // COLMOD: Pixel format
    write_cmd(PIXFMT);
    write_data(0x55);


    // Frame rate ctrl
    write_cmd(FRMCTR1);
    write_data((uint8_t*)FRMCTR1_D, 2);


    write_cmd(DFUNCTR);
    write_data((uint8_t*)DFUNCTR_D, 3);


    // Enable 3 gamma ctrl
    write_cmd(ENABLE3G);
    write_data(0x00);

    // Gamma curve selected
    write_cmd(GAMMASET);
    write_data(0x01);


    write_cmd(GMCTRP1);
    write_data((uint8_t*)GMCTRP1_D, 15);


    write_cmd(GMCTRN1);
    write_data((uint8_t*)GMCTRN1_D, 15);

    // Exit sleep
    write_cmd(SLPOUT);
    sleep_ms(100);

    // Turn on display
    write_cmd(DISPLAY_ON);

    // Clear display
    clear();
}

void ILI9341::scroll(uint16_t pixels)
{
    write_cmd(VSCRSADD);
    write_data(pixels >> 8);
    write_data(pixels & 0xFF);
}

uint16_t ILI9341::get_color(uint8_t r, uint8_t g, uint8_t b)
{
    uint16_t color = ((r & 0xf8) << 8 | (g & 0xfc) << 3 | b >> 3);
    uint8_t color_big[2] = {(uint8_t)(color >> 8), (uint8_t)(color & 0xFF)};
    uint16_t* retval = (uint16_t*)(color_big);
    return *retval;
}

void ILI9341::get_dimensions(uint16_t* width, uint16_t* height)
{
    *width = _WIDTH;
    *height = _HEIGHT;
}

void ILI9341::scroll_vertical(uint16_t pixels)
{
    _SCROLL_OFFSET += pixels;
    if (_SCROLL_OFFSET >= _HEIGHT)
        _SCROLL_OFFSET %= _HEIGHT;

    // Set the offset
    scroll(_HEIGHT - _SCROLL_OFFSET);
}