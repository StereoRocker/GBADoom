// Copyright 2021 Dominic Houghton. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// ILI9341 SPI display driver for Raspberry Pico, confirming to I_Framebuffer interface

#ifndef ILI9341_H
#define ILI9341_H

#include "hardware/spi.h"
#include "I_Framebuffer.hpp"

class ILI9341 : public I_Framebuffer<uint16_t> {
    public:
        // By default, we drive the SPI interface at 25MHz, I've had success with this.
        ILI9341(spi_inst* spiport, uint8_t miso, uint8_t mosi, uint8_t sck, uint8_t cs, uint8_t ds, uint8_t rst, uint16_t width=240, uint16_t height=320, uint16_t rotation=0, uint32_t baudrate=25*1000*1000);
        void set_parameters(uint16_t width, uint16_t height, uint16_t rotation);
        void reset();

        // len is expected in pixels, not bytes
        void plot_block(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t* pixeldata, uint32_t len);

        void plot_pixel(uint16_t x, uint16_t y, uint16_t color);
        void clear(uint16_t color = 0);

        // Please note - this function returns the RGB565 value in BIG ENDIAN, which is what the display expects, to allow arrays of uint16_t[] to be created without conversion.
        uint16_t get_color(uint8_t r, uint8_t g, uint8_t b);

        // Writes the display width & height to the variables specified
        void get_dimensions(uint16_t* width, uint16_t* height);

        void scroll(uint16_t pixels);
        void scroll_vertical(uint16_t pixels);
    
    private:
        // Private methods
        void initialise();
        void write_cmd(uint8_t command);
        void write_cmd(uint8_t command, uint8_t* data, uint32_t len);
        void write_data(uint8_t* data, uint32_t len);
        void write_data(uint8_t data);
        bool bounds(uint16_t x, uint16_t y);

        // Private variables
        spi_inst_t* _SPI;
        uint8_t     _MISO;
        uint8_t     _MOSI;
        uint8_t     _SCK;
        uint8_t     _CS;
        uint8_t     _DC;
        uint8_t     _RST;
        uint16_t    _WIDTH;
        uint16_t    _HEIGHT;
        uint8_t     _ROTATION;
        int16_t    _SCROLL_OFFSET;

        // Private constants
        const uint8_t NOP           = 0x00;  // No-op
        const uint8_t SWRESET       = 0x01;  // Software reset
        const uint8_t RDDID         = 0x04;  // Read display ID info
        const uint8_t RDDST         = 0x09;  // Read display status
        const uint8_t SLPIN         = 0x10;  // Enter sleep mode
        const uint8_t SLPOUT        = 0x11;  // Exit sleep mode
        const uint8_t PTLON         = 0x12;  // Partial mode on
        const uint8_t NORON         = 0x13;  // Normal display mode on
        const uint8_t RDMODE        = 0x0A;  // Read display power mode
        const uint8_t RDMADCTL      = 0x0B;  // Read display MADCTL
        const uint8_t RDPIXFMT      = 0x0C;  // Read display pixel format
        const uint8_t RDIMGFMT      = 0x0D;  // Read display image format
        const uint8_t RDSELFDIAG    = 0x0F;  // Read display self-diagnostic
        const uint8_t INVOFF        = 0x20;  // Display inversion off
        const uint8_t INVON         = 0x21;  // Display inversion on
        const uint8_t GAMMASET      = 0x26;  // Gamma set
        const uint8_t DISPLAY_OFF   = 0x28;  // Display off
        const uint8_t DISPLAY_ON    = 0x29;  // Display on
        const uint8_t SET_COLUMN    = 0x2A;  // Column address set
        const uint8_t SET_PAGE      = 0x2B;  // Page address set
        const uint8_t WRITE_RAM     = 0x2C;  // Memory write
        const uint8_t READ_RAM      = 0x2E;  // Memory read
        const uint8_t PTLAR         = 0x30;  // Partial area
        const uint8_t VSCRDEF       = 0x33;  // Vertical scrolling definition
        const uint8_t MADCTL        = 0x36;  // Memory access control
        const uint8_t VSCRSADD      = 0x37;  // Vertical scrolling start address
        const uint8_t PIXFMT        = 0x3A;  // COLMOD: Pixel format set
        const uint8_t FRMCTR1       = 0xB1;  // Frame rate control (In normal mode/full colors
        const uint8_t FRMCTR2       = 0xB2;  // Frame rate control (In idle mode/8 colors
        const uint8_t FRMCTR3       = 0xB3;  // Frame rate control (In partial mode/full colors
        const uint8_t INVCTR        = 0xB4;  // Display inversion control
        const uint8_t DFUNCTR       = 0xB6;  // Display function control
        const uint8_t PWCTR1        = 0xC0;  // Power control 1
        const uint8_t PWCTR2        = 0xC1;  // Power control 2
        const uint8_t PWCTRA        = 0xCB;  // Power control A
        const uint8_t PWCTRB        = 0xCF;  // Power control B
        const uint8_t VMCTR1        = 0xC5;  // VCOM control 1
        const uint8_t VMCTR2        = 0xC7;  // VCOM control 2
        const uint8_t RDID1         = 0xDA;  // Read ID 1
        const uint8_t RDID2         = 0xDB;  // Read ID 2
        const uint8_t RDID3         = 0xDC;  // Read ID 3
        const uint8_t RDID4         = 0xDD;  // Read ID 4
        const uint8_t GMCTRP1       = 0xE0;  // Positive gamma correction
        const uint8_t GMCTRN1       = 0xE1;  // Negative gamma correction
        const uint8_t DTCA          = 0xE8;  // Driver timing control A
        const uint8_t DTCB          = 0xEA;  // Driver timing control B
        const uint8_t POSC          = 0xED;  // Power on sequence control
        const uint8_t ENABLE3G      = 0xF2;  // Enable 3 gamma control
        const uint8_t PUMPRC        = 0xF7;  // Pump ratio control

        const uint8_t DISPLAY_ROTATE_0   = 0x88;
        const uint8_t DISPLAY_ROTATE_90  = 0xE8;
        const uint8_t DISPLAY_ROTATE_180 = 0x48;
        const uint8_t DISPLAY_ROTATE_270 = 0x28;

        const uint8_t PWCTRB_D[3] = {0x00, 0xC1, 0x30};
        const uint8_t POSC_D[4] = {0x64, 0x03, 0x12, 0x81};
        const uint8_t DTCA_D[3] = {0x85, 0x00, 0x78};
        const uint8_t PWCTRA_D[5] = {0x39, 0x2C, 0x00, 0x34, 0x02};
        const uint8_t DTCB_D[2] = {0x00, 0x00};
        const uint8_t VMCTR1_D[2] = {0x3E, 0x28};
        const uint8_t FRMCTR1_D[2] = {0x00, 0x18};
        const uint8_t DFUNCTR_D[3] = {0x08, 0x82, 0x27};
        const uint8_t GMCTRP1_D[15] = {0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E,
                                0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00};
        const uint8_t GMCTRN1_D[15] = {0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31,
                       0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F};
};
    


#endif