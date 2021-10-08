#include <stdarg.h>
#include <stdio.h>
#include <cstring>

//#define __arm__

#if PICO == 1

#include "pico/stdlib.h"

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


//**************************************************************************************


//*******************************************************************************
//VBlank handler.
//*******************************************************************************

void VBlankCallback()
{

}


void I_InitScreen_e32()
{

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

#include "pico/stdlib.h"

#define SHIFT_CLK 28
#define SHIFT_DAT 27
#define INPUT_DAT 26

#define BTN_UP    0x08
#define BTN_DOWN  0x04
#define BTN_LEFT  0x01
#define BTN_RIGHT 0x02

#define BTN_SL    0x20
#define BTN_SR    0x10
#define BTN_FIRE  0x40
#define BTN_SEL   0x80

// Reads the value of 8 inputs by pushing a high pin, then 7 low pins, sampling after each one
uint8_t shift_read()
{
    // Clock low
    // Shift data high
    // Clock high
    // Wait
    // Clock low
    // Shift data low
    // Read bit
    // Repeat 7 times:
    // - Clock high
    // - Wait
    // - Clock low
    // - Read bit
    uint8_t result = 0;

    gpio_put(SHIFT_CLK, 0);
    gpio_put(SHIFT_DAT, 1);
    sleep_us(1);
    gpio_put(SHIFT_CLK, 1);
    sleep_us(1);
    gpio_put(SHIFT_CLK, 0);
    gpio_put(SHIFT_DAT, 0);
    result = gpio_get(INPUT_DAT);
    for (int i = 0; i < 7; i++)
    {
        gpio_put(SHIFT_CLK, 1);
        sleep_us(1);
        gpio_put(SHIFT_CLK, 0);
        sleep_us(1);
        result = (result << 1) + gpio_get(INPUT_DAT);
    }

    return result;
}


void I_PollWServEvents_e32()
{
    static bool pl = false, pr = false, pu = false, pd = false, pf = false, ps = false, psl = false, psr = false;
    bool left, right, up, down, fire, sel, strafe_left, strafe_right;

    up = !gpio_get(PICOSYSTEM_SW_UP_PIN);
    down = !gpio_get(PICOSYSTEM_SW_DOWN_PIN);
    left = !gpio_get(PICOSYSTEM_SW_LEFT_PIN);
    right = !gpio_get(PICOSYSTEM_SW_RIGHT_PIN);

    fire = !gpio_get(PICOSYSTEM_SW_A_PIN);
    sel = !gpio_get(PICOSYSTEM_SW_B_PIN);
    strafe_left = !gpio_get(PICOSYSTEM_SW_Y_PIN);
    strafe_right = !gpio_get(PICOSYSTEM_SW_X_PIN);
    

    event_t ev;

    if (pu != up)
    {
        ev.data1 = KEYD_UP;
        ev.type = up ? ev_keydown : ev_keyup;
        D_PostEvent(&ev);
        pu = up;
    }

    if (pd != down)
    {
        ev.data1 = KEYD_DOWN;
        ev.type = down ? ev_keydown : ev_keyup;
        D_PostEvent(&ev);
        pd = down;
    }

    if (pl != left)
    {
        ev.data1 = KEYD_LEFT;
        ev.type = left ? ev_keydown : ev_keyup;
        D_PostEvent(&ev);
        pl = left;
    }

    if (pr != right)
    {
        ev.data1 = KEYD_RIGHT;
        ev.type = right ? ev_keydown : ev_keyup;
        D_PostEvent(&ev);
        pr = right;
    }

    if (pf != fire)
    {
        ev.data1 = KEYD_B;
        ev.type = fire ? ev_keydown : ev_keyup;
        D_PostEvent(&ev);
        pf = fire;
    }

    if (ps != sel)
    {
        ev.data1 = KEYD_A;
        ev.type = sel ? ev_keydown : ev_keyup;
        D_PostEvent(&ev);
        ps = sel;
    }

    if (psl != strafe_left)
    {
        ev.data1 = KEYD_L;
        ev.type = strafe_left ? ev_keydown : ev_keyup;
        D_PostEvent(&ev);
        psl = strafe_left;
    }

    if (psr != strafe_right)
    {
        ev.data1 = KEYD_R;
        ev.type = strafe_right ? ev_keydown : ev_keyup;
        D_PostEvent(&ev);
        psr = strafe_right;
    }
}

//**************************************************************************************

void I_ClearWindow_e32()
{

}

//**************************************************************************************

extern uint8_t frame[];

unsigned char* I_GetBackBuffer()
{
   return (unsigned char*)frame;
}

//**************************************************************************************

void I_CreateWindow_e32()
{

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
        //uint8_t color_big[2] = {(uint8_t)(frame_palette[i] >> 8), (uint8_t)(frame_palette[i] & 0xFF)};
        //frame_palette[i] = *(uint16_t*)(color_big);
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
