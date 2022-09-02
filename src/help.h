#ifndef __HELP_H_
#define __HELP_H_
#include <ti/getcsc.h>

#include "main.h"

bool help_draw()
{
    gfx_ZeroScreen();
    gfx_SetTextFGColor(8);
    gfx_SetTextScale(3, 3);
    gfx_PrintStringXY("Block Dude v 0.3", 2, 2);
    gfx_SetTextScale(1, 1);
    gfx_SetTextFGColor(5);
    gfx_PrintStringXY("Merthsoft Creations 2022", 2, 26);

    gfx_SetTextScale(1, 2);
    gfx_PrintStringXY("The goal of this game is to get from the start", 2, 40);
    gfx_PrintStringXY("to the door.", 2, 56);
    gfx_PrintStringXY("Left/Right to change direction and move", 2, 72);
    gfx_PrintStringXY("2nd to pickup and put down a block", 2, 88);
    gfx_PrintStringXY("Up to walk up one block", 2, 104);
    gfx_PrintStringXY("Mode to move the camera", 2, 120);
    gfx_PrintStringXY("Alpha to undo one move max", 2, 136);
    gfx_PrintStringXY("Del to go back", 2, 162);

    gfx_SetTextScale(1, 1);
    gfx_PrintStringXY("Programming - Shaun McFall", 2, GFX_LCD_HEIGHT - 28);
    gfx_PrintStringXY("Graphics - Aleksandr Makarov", 2, GFX_LCD_HEIGHT - 19);
    gfx_PrintStringXY("Levels and original game - Brandon Sterner", 2, GFX_LCD_HEIGHT - 10);

    return false;
}

enum state_transition help_update(sk_key_t key)
{
    return key == sk_Del ? st_back : st_none;
}

#endif