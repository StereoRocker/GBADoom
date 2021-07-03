#include <stdarg.h>
#include <stdio.h>
#include <cstring>

#include "pico/stdlib.h"

//#define __arm__

#ifdef __arm__

extern "C"
{
    #include "doomdef.h"
    #include "doomtype.h"
    #include "d_main.h"
    #include "d_event.h"

    #include "global_data.h"

    #include "tables.h"
}

#include "i_system_e32.h"

/*
#include "lprintf.h"

#include <gba.h>
#include <gba_input.h>
#include <gba_timers.h>

#include <maxmod.h>

#define DCNT_PAGE 0x0010

#define VID_PAGE1 VRAM
#define VID_PAGE2 0x600A000

#define TM_FREQ_1024 0x0003
#define TM_ENABLE 0x0080
#define TM_CASCADE 0x0004
#define TM_FREQ_1024 0x0003
#define TM_FREQ_256 0x0002

#define REG_WAITCNT	*((vu16 *)(0x4000204))

*/
//**************************************************************************************


//*******************************************************************************
//VBlank handler.
//*******************************************************************************

void VBlankCallback()
{
    //mmVBlank();
    //mmFrame();
}


void I_InitScreen_e32()
{
    /*
    irqInit();

    irqSet( IRQ_VBLANK, VBlankCallback );
    irqEnable(IRQ_VBLANK);


    //Set gamepak wait states and prefetch.
    REG_WAITCNT = 0x46DA;

    consoleDemoInit();

    REG_TM2CNT_L= 65535-1872;     // 1872 ticks = 1/35 secs
    REG_TM2CNT_H = TM_FREQ_256 | TM_ENABLE;       // we're using the 256 cycle timer

    // cascade into tm3
    REG_TM3CNT_H = TM_CASCADE | TM_ENABLE;
    */
}

//**************************************************************************************

void I_BlitScreenBmp_e32()
{

}

//**************************************************************************************

void I_StartWServEvents_e32()
{

}

//**************************************************************************************

#include "hardware/adc.h"

#define ADC_AX   0
#define PIN_AX   26

#define ADC_AY   1
#define PIN_AY   27

#define PIN_SEL  28
#define PIN_FIRE 29

void I_PollWServEvents_e32()
{
    bool left, right, up, down, fire, sel;

    adc_select_input(ADC_AX);
    uint adc_x_raw = adc_read();
    adc_select_input(ADC_AY);
    uint adc_y_raw = adc_read();

    fire = !gpio_get(PIN_FIRE);
    sel = !gpio_get(PIN_SEL);

    left = (adc_x_raw < 600);
    right = (adc_x_raw > 4000);
    up = (adc_y_raw > 4000);
    down = (adc_y_raw < 600);

    event_t ev;

    ev.data1 = KEYD_UP;
    ev.type = up ? ev_keydown : ev_keyup;
    D_PostEvent(&ev);

    ev.data1 = KEYD_DOWN;
    ev.type = down ? ev_keydown : ev_keyup;
    D_PostEvent(&ev);

    ev.data1 = KEYD_LEFT;
    ev.type = left ? ev_keydown : ev_keyup;
    D_PostEvent(&ev);

    ev.data1 = KEYD_RIGHT;
    ev.type = right ? ev_keydown : ev_keyup;
    D_PostEvent(&ev);

    ev.data1 = KEYD_B;
    ev.type = fire ? ev_keydown : ev_keyup;
    D_PostEvent(&ev);

    ev.data1 = KEYD_A;
    ev.type = sel ? ev_keydown : ev_keyup;
    D_PostEvent(&ev);
}

//**************************************************************************************

void I_ClearWindow_e32()
{

}

//**************************************************************************************

extern uint8_t frame[];

unsigned char* I_GetBackBuffer()
{
    /*
    if(REG_DISPCNT & DCNT_PAGE)
        return (unsigned short*)VID_PAGE1;

    return (unsigned short*)VID_PAGE2;
    */
   return (unsigned char*)frame;
}

//**************************************************************************************

void I_CreateWindow_e32()
{
    /*
    //Bit5 = unlocked vram at h-blank.
    SetMode(MODE_4 | BG2_ENABLE | BIT(5));

    unsigned short* bb = I_GetBackBuffer();

    memset(bb, 0, 240*160);

    I_FinishUpdate_e32(NULL, NULL, 0, 0);

    bb = I_GetBackBuffer();

    memset(bb, 0, 240*160);

    I_FinishUpdate_e32(NULL, NULL, 0, 0);
    */
}

//**************************************************************************************

void I_CreateBackBuffer_e32()
{
    I_CreateWindow_e32();
}

//**************************************************************************************

void render_fb();

void I_FinishUpdate_e32(const byte* srcBuffer, const byte* pallete, const unsigned int width, const unsigned int height)
{
    //REG_DISPCNT ^= DCNT_PAGE;
    render_fb();
}

//**************************************************************************************

extern uint16_t frame_palette[256];

void I_SetPallete_e32(const byte* pallete)
{

    for(int i = 0; i< 256; i++)
    {
        byte r = *pallete++;
        byte g = *pallete++;
        byte b = *pallete++;

        frame_palette[i] = ((r & 0xf8) << 8 | (g & 0xfc) << 3 | b >> 3);
        uint8_t color_big[2] = {(uint8_t)(frame_palette[i] >> 8), (uint8_t)(frame_palette[i] & 0xFF)};
        frame_palette[i] = *(uint16_t*)(color_big);
    }
}

//**************************************************************************************

int I_GetVideoWidth_e32()
{
    return 120;
}

//**************************************************************************************

int I_GetVideoHeight_e32()
{
    return 160;
}



//**************************************************************************************

void I_ProcessKeyEvents()
{
    I_PollWServEvents_e32();
}

//**************************************************************************************

#define MAX_MESSAGE_SIZE 1024

void I_Error (const char *error, ...)
{
    
    //consoleDemoInit();

    char msg[MAX_MESSAGE_SIZE];

    va_list v;
    va_start(v, error);

    vsprintf(msg, error, v);

    va_end(v);

    printf("%s", msg);

    while(true)
    {
        __breakpoint();
        //VBlankIntrWait();
    }
}

//**************************************************************************************

void I_Quit_e32()
{

}

//**************************************************************************************

#endif
