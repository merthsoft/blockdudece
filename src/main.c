#include <graphx.h>
#include <ti/getcsc.h>
#include <debug.h>
#include <sys/timers.h>
#include <string.h>
#include <fileioc.h>

#include "gfx/gfx.h"
#include "level.h"
#include "effects.h"
#include "main.h"
#include "menu.h"
#include "game.h"
#include "help.h"
#include "win.h"

gfx_tilemap_t tilemap;

bool fade_flag = true;

uint16_t *palette;
uint8_t palette_size;

enum game_state game_state;

#define copywrite_length 24

gfx_sprite_t* arrow_right;
gfx_sprite_t* arrow_down;
gfx_sprite_t* dude_right;

int main(void)
{
    gfx_Begin();
    gfx_SetDrawBuffer();
    gfx_ZeroScreen();

    arrow_right = gfx_MallocSprite(8, 16);
    gfx_FlipSpriteY(arrow_left, arrow_right);

    arrow_down = gfx_MallocSprite(18, 8);
    gfx_FlipSpriteX(arrow_up, arrow_down);

    dude_right = gfx_MallocSprite(8, 8);
    gfx_FlipSpriteY(dude, dude_right);

    memset(gfx_palette, 0, 512);
    int sizeof_base_palette = sizeof_global_palette / 2;

    palette_size = sizeof_base_palette + copywrite_length;

    palette = malloc(sizeof_global_palette + copywrite_length * 2);
    memcpy(palette, global_palette, sizeof_global_palette);

    for (int i = 0; i < copywrite_length; i++)
        palette[i + sizeof_base_palette] = HsvToRgb(i * (255 / copywrite_length), 255, 255);

    levels_load();

    uint8_t var = ti_Open("[BD", "r");
    if (var)
    {
        ti_Read(move_count_list, 2, NUM_BUILTIN_LEVELS, var);
        ti_Close(var);
    }

    tilemap.tile_height = 16;
    tilemap.tile_width = 16;
    tilemap.type_height = gfx_tile_16_pixel;
    tilemap.type_width = gfx_tile_16_pixel;
    tilemap.tiles = tiles_tiles;

    menu_load(&tilemap);

    gfx_ZeroScreen();
    gfx_Tilemap(&tilemap, 0, 0);

    bool exit = false;
    game_state = gs_main_menu;
    while (!exit)
    {
        int key = os_GetCSC();
        enum state_transition state_transition = st_none;

        switch (game_state)
        {
            case gs_main_menu:
                state_transition = main_menu_update(key);
                break;
            case gs_level_select:
                state_transition = level_select_update(key, &tilemap);
                break;
            case gs_playing:
                state_transition = game_update(key, &tilemap);
                break;
            case gs_help:
                state_transition = help_update(key);
                break;
            case gs_win:
                state_transition = win_update(key);
                break;
            default:
                state_transition = (key == sk_Del) ? st_back : st_none;
                break;
        }

        switch (state_transition)
        {
            case st_none:
                break;
            case st_back:
                switch (game_state)
                {
                    case gs_main_menu:
                        exit = true;
                        fade_flag = false;
                        break;
                    case gs_level_select:
                    case gs_playing:
                    case gs_help:
                    case gs_win:
                        fade_flag = true;
                        game_state = gs_main_menu;
                        menu_load(&tilemap);
                        break;
                }
                break;
            case st_fade_out:
                fade_flag = true;
                break;
            default:
                fade_flag = true;
                game_state = (enum game_state)state_transition;
                if (game_state == gs_level_select)
                    level_select_start(&tilemap);
                break;
        }

        if (fade_flag)
            fade_out(palette, 0, palette_size); 

        gfx_ZeroScreen();

        switch (game_state)
        {
            case gs_main_menu:
                main_menu_draw(&tilemap);
                break;
            case gs_level_select:
                level_select_draw(&tilemap);
                break;
            case gs_playing:
                game_draw(&tilemap);
                break;
            case gs_help:
                help_draw();
                break;
            case gs_win:
                win_draw();
                break;
        }

        gfx_SwapDraw();

        if (fade_flag) 
        { 
            fade_in(palette, 0, palette_size); 
            fade_flag = false; 
        }
    }

    var = ti_Open("[BD", "w+");
    if (var)
    {
        ti_Write(move_count_list, 2, NUM_BUILTIN_LEVELS, var);
        ti_SetArchiveStatus(true, var);
        ti_Close(var);
    }

    free(dude_right);
    free(arrow_down);
    free(arrow_right);

    if (tilemap.map)
        free(tilemap.map);

    levels_free();

    gfx_End();

    return 0;
}