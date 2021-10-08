// Copyright 2021 Dominic Houghton. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#if PICO == 1

#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "pico/st7789.h"

#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"

// Audio
extern "C" {
#include "sounds.h"
#include "w_wad.h"
}
#include "pwm_audio.h"

extern "C" {
volatile soundChannel_t soundChannels[MAX_CHANNELS];
}

/*
 * PWM Interrupt Handler which outputs PWM level and advances the 
 * current sample. 
 * 
 * We repeat the same value for 8 cycles this means sample rate etc
 * adjust by factor of 8   (this is what bitshifting <<3 is doing)
 * 
 */
void pwm_interrupt_handler() {
    
    uint16_t pwm = 0;
    uint16_t sample = 0;

    // Now, for each channel we need to copy sample data.
    for (int ch = 0; ch < MAX_CHANNELS; ch++)
    {
        // channel active?
        if (soundChannels[ch].lump != NULL)
        {
            if (soundChannels[ch].offset >= (soundChannels[ch].size<<3))
            {
                soundChannels[ch].volume = 0;
                soundChannels[ch].lump = NULL;
                continue;
            }
            pwm += (soundChannels[ch].lump[soundChannels[ch].offset>>3]);
            soundChannels[ch].offset++;
        }
    }

    pwm_clear_irq(pwm_gpio_to_slice_num(PICOSYSTEM_AUDIO_PIN));
    pwm_set_gpio_level(PICOSYSTEM_AUDIO_PIN, (pwm>>3)&0xFF);
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


// lcd configuration
const struct st7789_config lcd_config = {
    .spi      = spi0,
    .gpio_din = PICOSYSTEM_LCD_MOSI_PIN,
    .gpio_clk = PICOSYSTEM_LCD_SCLK_PIN,
    .gpio_cs  = PICOSYSTEM_LCD_CSN_PIN,
    .gpio_dc  = PICOSYSTEM_LCD_DC_PIN,
    .gpio_rst = PICOSYSTEM_LCD_RESET_PIN,
    .gpio_bl  = PICOSYSTEM_BACKLIGHT_PIN,
};

// Currently unable to go larger - E1M3 fails to load (ZMalloc fails to allocate 504 bytes) with 320x240px buffer
uint8_t frame[240*160];
uint16_t frame_palette[256];

// Picosystem display is 240x240px
const int Y_OFFSET = ((240 - 160) / 2);
const int X_OFFSET = 0;
const int X_LIMIT  = (X_OFFSET + 239);

extern "C" void c_main(uint8_t* fb);

void render_fb(void)
{
    // Here's hoping the stack has 480 bytes free for me lmao
    uint16_t line[240];

    // Set up the LCD
    st7789_set_cursor(X_OFFSET, Y_OFFSET);



    // For each line
    for (int i = 0; i < 160; i++)
    {
        // For each pixel on the line
        for (int j = 0; j < 240; j++)
        {
            line[j] = frame_palette[frame[(i*240) + j]];
        }

        // Output the line to display
        st7789_write(line, 480);

        // Output the line to display
        //display->plot_block(X_OFFSET , i+Y_OFFSET, X_LIMIT, i+Y_OFFSET+1, line, 240);
    }
}

#define PLL_SYS_KHZ (176 * 1000)

extern char __flash_binary_end;

int main()
{
    #if 0
    // Overclock to 176MHz
    set_sys_clock_khz(PLL_SYS_KHZ, true); 

    // clk_peri is now attached to pll_usb, so its maximum speed is 24Mhz
    // Reassign clk_peri to clk_sys with no divider
    clock_configure(
        clk_peri,
        0,                                                // No glitchless mux
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, // System PLL on AUX mux
        PLL_SYS_KHZ * 1000,                               // Input frequency
        PLL_SYS_KHZ * 1000                                // Output (must be same as no divider)
    );
    #endif

    // Set all pins high, to avoid chip selecting an incorrect device
    for (int pin = 0; pin < 29; pin++)
    {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_OUT);
        gpio_put(pin, 1);
    }
    

    // Init audio

    
    //memset(audioBuffer, 0, AUDIO_BUF_LEN);    // Clear buffer
    memset((void*)soundChannels, 0, sizeof(soundChannels));

    gpio_set_function(PICOSYSTEM_AUDIO_PIN, GPIO_FUNC_PWM);
    int audio_pin_slice = pwm_gpio_to_slice_num(PICOSYSTEM_AUDIO_PIN);

    
    // Setup PWM interrupt to fire when PWM cycle is complete
    pwm_clear_irq(audio_pin_slice);
    pwm_set_irq_enabled(audio_pin_slice, true);
    // set the handle function above
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler); 
    irq_set_enabled(PWM_IRQ_WRAP, true);
   

    // Setup PWM for audio output
    pwm_config config = pwm_get_default_config();
    /* Base clock 176,000,000 Hz divide by wrap 250 then the clock divider further divides
     * to set the interrupt rate. 
     * 
     * 11 KHz is fine for speech. Phone lines generally sample at 8 KHz
     * 
     * 
     * So clkdiv should be as follows for given sample rate
     *  8.0f for 11 KHz
     *  4.0f for 22 KHz
     *  2.0f for 44 KHz etc
     */
    
    pwm_config_set_clkdiv(&config, 8.0f); 
    pwm_config_set_wrap(&config, 250); 
    pwm_init(audio_pin_slice, &config, true);

    pwm_set_gpio_level(PICOSYSTEM_AUDIO_PIN, 0);

    // Initialise all SDK-provided stdio drivers
    stdio_init_all();
    
    printf("Set up PWM\n");

    // Initialise ILI9341 display
    st7789_init(&lcd_config, 240, 240);
    st7789_fill(0x0000);
    printf("Initialised st7789\n");

    // Set up static framebuffer
    memset(frame, 0, 240*160);

    

    // Output details on binary
    printf("\n\n####################\nBinary end: %x\n####################\n\n", ((uintptr_t) &__flash_binary_end));

    c_main(frame);
    
    while (1)
    {
        printf("Reached end\n");
        sleep_ms(1000);
    }
    // Break, and halt execution
    for(;;);
    __breakpoint();
    for(;;);
    return 0;
}

#endif