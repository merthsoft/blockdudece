#ifndef __WIN_H_
#define __WIN_H_

#include "main.h"

int dance = 0;

enum state_transition win_update(sk_key_t key)
{
    dance = (dance + 1) % 20;
    return key ? st_back : st_none;
}

void win_draw()
{
    gfx_SetTextFGColor(8);
    gfx_SetTextScale(3, 3);

    int w = gfx_GetStringWidth("You win!");
    gfx_PrintStringXY("You win!", (GFX_LCD_WIDTH - w) / 2, (GFX_LCD_HEIGHT - 24) / 2);
    gfx_SetTextFGColor(5);
    gfx_SetTextScale(1, 1);
    gfx_PrintStringXY("press any key", (GFX_LCD_WIDTH - w) / 2, (GFX_LCD_HEIGHT - 24) / 2 + 26);

    gfx_ScaledSprite_NoClip(dance < 10 ? dude_right : dude, 128, 2, 8, 8);
}

#endif