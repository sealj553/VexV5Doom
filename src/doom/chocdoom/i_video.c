// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// $Log:$
//
// DESCRIPTION:
//	DOOM graphics stuff for X11, UNIX.
//
//-----------------------------------------------------------------------------

static const char rcsid[] = "$Id: i_x.c,v 1.6 1997/02/03 22:45:10 b1 Exp $";

#include "config.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_event.h"
#include "d_main.h"
#include "i_video.h"
#include "z_zone.h"

#include "tables.h"
#include "doomkeys.h"

#include <stdint.h>
#include <stdbool.h>

#include "pros/apix.h"
#include "api.h"

extern int key_nextweapon;
extern int key_prevweapon;

const int x_offset = (LV_HOR_RES - SCREENWIDTH)/2;
const int y_offset = (LV_VER_RES - SCREENHEIGHT)/2;

void update_controller(void);
void check_button(int prev, int curr, int action);

struct controller_state {
    int32_t
        a_lx,
        a_ly,
        a_rx,
        a_ry,
        d_l1,
        d_l2,
        d_r1,
        d_r2,
        d_up,
        d_dwn,
        d_lft,
        d_rig,
        d_x,
        d_b,
        d_y,
        d_a;
}; 

static struct controller_state c_state = { 0 };
static struct controller_state c_oldstate = { 0 };
static event_t event;

lv_vdb_t *framebuffer;

// The screen buffer; this is modified to draw things to the screen

byte *I_VideoBuffer = NULL;

// If true, game is running as a screensaver

boolean screensaver_mode = false;

// Gamma correction level to use
int usegamma = 0;

// Mouse acceleration
//
// This emulates some of the behavior of DOS mouse drivers by increasing
// the speed when the mouse is moved fast.
//
// The mouse input values are input directly to the game, but when
// the values exceed the value of mouse_threshold, they are multiplied
// by mouse_acceleration to increase the speed.

float mouse_acceleration = 2.0;
int mouse_threshold = 10;

int usemouse = 0;

// If true, keyboard mapping is ignored, like in Vanilla Doom.
// The sensible thing to do is to disable this if you have a non-US
// keyboard.

int vanilla_keyboard_mapping = true;

typedef struct {
    byte r;
    byte g;
    byte b;
} col_t;

// Palette converted to RGB888
static lv_color_t rgb888_palette[256];

// run state
static bool run;

void I_InitGraphics (void)
{
    I_VideoBuffer = (byte*)Z_Malloc (SCREENWIDTH * SCREENHEIGHT, PU_STATIC, NULL);

    framebuffer = lv_vdb_get();
    memset((*framebuffer).buf, 0, LV_HOR_RES * LV_VER_RES * sizeof(lv_color_t));
}

void I_ShutdownGraphics (void)
{
    Z_Free (I_VideoBuffer);
}

void I_StartFrame (void) { }

void update_controller(void){
    c_state.a_lx  = controller_get_analog(E_CONTROLLER_MASTER,  E_CONTROLLER_ANALOG_LEFT_X);
    c_state.a_ly  = controller_get_analog(E_CONTROLLER_MASTER,  E_CONTROLLER_ANALOG_LEFT_Y);
    c_state.a_rx  = controller_get_analog(E_CONTROLLER_MASTER,  E_CONTROLLER_ANALOG_RIGHT_X);
    c_state.a_ry  = controller_get_analog(E_CONTROLLER_MASTER,  E_CONTROLLER_ANALOG_RIGHT_Y);
    c_state.d_l1  = controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_L1);
    c_state.d_l2  = controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_L2);
    c_state.d_r1  = controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_R1);
    c_state.d_r2  = controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_R2);
    c_state.d_up  = controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_UP);
    c_state.d_dwn = controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_DOWN);
    c_state.d_lft = controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_LEFT);
    c_state.d_rig = controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_RIGHT);
    c_state.d_x   = controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_X);
    c_state.d_b   = controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_B);
    c_state.d_y   = controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_Y);
    c_state.d_a   = controller_get_digital(E_CONTROLLER_MASTER, E_CONTROLLER_DIGITAL_A);
}

void check_button(int prev, int curr, int action){
    if(!prev && curr){
        event.type = ev_keydown;
        event.data1 = action; //key code
        D_PostEvent(&event);
    } else if(prev && !curr){
        event.type = ev_keyup;
        event.data1 = action;
        D_PostEvent(&event);
    }
}

void I_GetEvent (void)
{
    c_oldstate = c_state;
    update_controller();

    //joystick and strafe
    //left x = turn left/right
    //left y = move forward/back
    //right x = strafe left/right
    event.type = ev_joystick;
    event.data1 = 0; //bitfield of buttons
    event.data2 = c_state.a_lx; //x axis mouse (turn)
    event.data3 = -c_state.a_ly; //y axis mouse (forward/backward)
    event.data4 = c_state.a_rx; //3rd axis mouse (strafe)
    D_PostEvent(&event);

    //other buttons
    //a = fire
    check_button(c_oldstate.d_a, c_state.d_a, KEY_FIRE);
    //b = use
    check_button(c_oldstate.d_b, c_state.d_b, KEY_USE);
    //x = enter
    check_button(c_oldstate.d_x, c_state.d_x, KEY_ENTER);
    //y = escape
    check_button(c_oldstate.d_y, c_state.d_y, KEY_ESCAPE);

    //dpad is arrow keys
    check_button(c_oldstate.d_up,  c_state.d_up,  KEY_UPARROW);
    check_button(c_oldstate.d_dwn, c_state.d_dwn, KEY_DOWNARROW);
    check_button(c_oldstate.d_lft, c_state.d_lft, KEY_LEFTARROW);
    check_button(c_oldstate.d_rig, c_state.d_rig, KEY_RIGHTARROW);

    //l1 = prev weapon
    check_button(c_oldstate.d_l1, c_state.d_l1, key_prevweapon);

    //r1 = next weapon
    check_button(c_oldstate.d_r1, c_state.d_r1, key_nextweapon);
}

void I_StartTic (void) {
    I_GetEvent();
}

void I_UpdateNoBlit(void){}

//scale video height 240
void I_FinishUpdate(void){
    for (int y = 0; y < SCREENHEIGHT; ++y) {
        for (int x = 0; x < SCREENWIDTH; ++x) {
            byte index = I_VideoBuffer[y * SCREENWIDTH + x];
            (*framebuffer).buf[(y + y_offset) * LV_HOR_RES + x + x_offset] = rgb888_palette[index];
        }
    }
    lv_vdb_flush();
}

//
// I_ReadScreen
//
void I_ReadScreen (byte* scr)
{
    memcpy (scr, I_VideoBuffer, SCREENWIDTH * SCREENHEIGHT);
}

//
// I_SetPalette
//
void I_SetPalette (byte* palette)
{
    for(int i = 0; i < 256; ++i){
        rgb888_palette[i].red   = gammatable[usegamma][palette[0]];
        rgb888_palette[i].green = gammatable[usegamma][palette[1]];
        rgb888_palette[i].blue  = gammatable[usegamma][palette[2]];
        rgb888_palette[i].alpha = 0;

        palette += 3;
    }
}

// Given an RGB value, find the closest matching palette index.

int I_GetPaletteIndex (int r, int g, int b)
{
    int best = 0;
    int best_diff = INT_MAX;

    for (int i = 0; i < 256; ++i) {
        int diff = 
            (r - rgb888_palette[i].red   ) * (r - rgb888_palette[i].red   ) +
            (b - rgb888_palette[i].blue  ) * (b - rgb888_palette[i].blue  ) +
            (g - rgb888_palette[i].green ) * (g - rgb888_palette[i].green );

        if (diff < best_diff) {
            best = i;
            best_diff = diff;
        }

        if (diff == 0) {
            break;
        }
    }

    return best;
}

void I_BeginRead (void) { }

void I_EndRead (void) { }

void I_SetWindowTitle (char *title) { }

void I_GraphicsCheckCommandLine (void) { }

void I_SetGrabMouseCallback (grabmouse_callback_t func) { }

void I_EnableLoadingDisk (void) { }

void I_BindVideoVariables (void) { }

void I_DisplayFPSDots (boolean dots_on) { }

void I_CheckIsScreensaver (void) { }
