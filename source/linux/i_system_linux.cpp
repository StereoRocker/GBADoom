#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <cstring>

//#define __arm__

#if LINUX == 1

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




void I_PollWServEvents_e32()
{
    return; 

    /*
    static bool pl = false, pr = false, pu = false, pd = false, pf = false, ps = false, psl = false, psr = false;
    bool left, right, up, down, fire, sel, strafe_left, strafe_right;

    uint8_t buttons = 0; 

    up = buttons & BTN_UP;
    down = buttons & BTN_DOWN;
    left = buttons & BTN_LEFT;
    right = buttons & BTN_RIGHT;

    fire = buttons & BTN_FIRE;
    sel = buttons & BTN_SEL;
    strafe_left = buttons & BTN_SL;
    strafe_right = buttons & BTN_SR;
    

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
    */
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
        /*uint8_t color_big[2] = {(uint8_t)(frame_palette[i] >> 8), (uint8_t)(frame_palette[i] & 0xFF)};
        frame_palette[i] = *(uint16_t*)(color_big);*/
    }
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

    char msg[MAX_MESSAGE_SIZE];

    va_list v;
    va_start(v, error);

    vsprintf(msg, error, v);

    va_end(v);

    printf("%s\n", msg);

    exit(-1);
}

//**************************************************************************************

void I_Quit_e32()
{

}

//**************************************************************************************

#endif
