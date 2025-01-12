#include <ti/getcsc.h>
#include <graphx.h>

#include "main.h"
#include "menu.h"
#include "level.h"
#include "gfx/gfx.h"

int choice = 0;

int copywrite_shimmer = 0;
const char copywrite[] = "2025 Merthsoft Creations";

const char *chars = "\0\0\0\0\0\0\0\0\0\0\"WRMH\0\0?[VQLG\0\0:ZUPKFC\0 YTOJEB\0\0XSNIDA\0\0\0\0\0\0\0\0";
const char *cheat = "SOPHIA";
int cheat_index = 0;

#define copywrite_length 24

void menu_load(gfx_tilemap_t* tilemap)
{
    struct level *level = &title_screen;
    tilemap->width = tilemap->draw_width = level->width;
    tilemap->height = tilemap->draw_height = level->height;
    
    if (tilemap->map)
        free(tilemap->map);
    tilemap->map = malloc(level->width * level->height);
    memcpy(tilemap->map, level->map_pointer, level->width * level->height);
    
    tilemap->x_loc = 0;
    tilemap->y_loc = 16;
}

enum state_transition main_menu_update(sk_key_t key)
{
    if (cheat_index == 6)
    {
        if (key)
            cheat_index = 0;
        return st_none;
    }
    
    bool exit = false;
    switch (key)
    {
        case sk_Del:
            exit = true;
            break;
        case sk_Left:
            cheat_index = 0;
            if (choice > 0)
                choice--;
            break;
        case sk_Right:
            cheat_index = 0;
            if (choice < 2)
                choice++;
            break;
        case sk_2nd:
        case sk_Enter:
            cheat_index = 0;
            switch (choice)
            {
                case 0:
                    return st_level_select;
                case 1:
                    return st_help;
                case 2:
                    exit = true;
                    break;
            }
        case 0:
            break;
        default:
            if (chars[key] != cheat[cheat_index++])
                cheat_index = 0;
            if (cheat_index == 6)
            {
                for (int i = 0; i < NUM_BUILTIN_LEVELS; i++)
                {
                    if (move_count_list[i] == 0)
                        move_count_list[i] = UINT16_MAX;
                }
            }
            break;
    }

    return exit ? st_back : st_none;
}

void main_menu_draw(gfx_tilemap_t* tilemap)
{
    gfx_Tilemap(tilemap, 0, 0);

    gfx_SetTextScale(2, 2);
    gfx_SetTextFGColor(8);
    int w = gfx_GetStringWidth("PLAY");

    int cursor_x = choice * (GFX_LCD_WIDTH / 2 - w / 2 - 2);
    int cursor_y = GFX_LCD_HEIGHT - 26;

    gfx_SetColor(5);
    gfx_FillRectangle(cursor_x, cursor_y, w + 2, 18);

    gfx_PrintStringXY("PLAY", 2, GFX_LCD_HEIGHT - 24);
    gfx_PrintStringXY("HELP", GFX_LCD_WIDTH / 2 - w / 2, GFX_LCD_HEIGHT - 24);
    gfx_PrintStringXY("QUIT", GFX_LCD_WIDTH - w - 2, GFX_LCD_HEIGHT - 24);

    gfx_SetTextScale(1, 1);
    w = gfx_GetStringWidth(copywrite);

    for (int i = 0; i < copywrite_length; i++)
    {
        int palette_index = (i + copywrite_shimmer) % copywrite_length;
        gfx_SetTextFGColor(palette_index + sizeof_global_palette / 2);
        gfx_SetTextXY(GFX_LCD_WIDTH / 2 - w / 2 + 8 * i, 2);
        gfx_PrintChar(copywrite[i]);
    }

    if (copywrite_shimmer < copywrite_length)
        copywrite_shimmer++;

    if (cheat_index == 6)
    {
        gfx_SetTextFGColor(32);
        gfx_SetTextBGColor(1);
        gfx_SetTextScale(3, 3);
        w = gfx_GetStringWidth("I love you <3");
        gfx_PrintStringXY("I love you <3", (GFX_LCD_WIDTH - w) / 2, (GFX_LCD_HEIGHT - 24) / 2);
        gfx_SetTextBGColor(255);
    }
}