#ifndef __MAIN_H_
#define __MAIN_H_

#include <graphx.h>
#include <debug.h>

#include "gfx/gfx.h"

enum game_state { gs_main_menu = 1, gs_level_select, gs_playing, gs_help, gs_win };

enum state_transition
{
    st_fade_out = -2,
    st_back = -1,
    st_none = 0,
    st_main_menu = gs_main_menu,
    st_level_select = gs_level_select,
    st_playing = gs_playing,
    st_help = gs_help,
    st_win = gs_win
};

extern gfx_sprite_t* arrow_right;
extern gfx_sprite_t* arrow_down;
extern gfx_sprite_t* dude_right;

#endif