#include <stdint.h>
#include <stdbool.h>
#include <ti/getcsc.h>
#include <graphx.h>
#include <string.h>
#include <debug.h>

#include "main.h"
#include "game.h"
#include "level.h"
#include "gfx/gfx.h"
#include "camera.h"

#define EMPTY 0
#define WALL 1
#define BLOCK 2
#define DOOR 3

enum last_move
{
    none,
    move,
    drop
};

int current_level = 0;
int player_y = 5;
int player_x = 17;
int level_width = 0;
int level_height = 0;
bool direction = false;
bool holding_block = false;
uint16_t move_count = 0;
int max_level = 0;

enum last_move last_move;
int undo_x = 0;
int undo_y = 0;

bool new_best = false;
bool reset = false;
uint8_t level_skip = 0;

#define player_draw_x(x_loc) (player_x * 16 + x_loc - camera->x)
#define player_draw_y(y_loc) (player_y * 16 + y_loc - camera->y)

char buffer[50];

const uint8_t win_cheat[] = { sk_Up, sk_Up, sk_Down, sk_Down, sk_Left, sk_Right, sk_Left, sk_Right, sk_2nd, sk_Alpha };
uint8_t win_cheat_index = 0;
#define CHEAT_LENGTH 10

void camera_reset(camera* camera, gfx_tilemap_t *tilemap) {
    camera->bounary_mode = cbm_strict;
    camera->move_mode = cmm_follow;
    camera->x = camera->min_x = 0;
    camera->y = camera->min_y = 0;
    camera->step_x =  tilemap->tile_height;
    camera->step_y = tilemap->tile_width;
    camera->viewport_width = GFX_LCD_WIDTH - tilemap->x_loc;
    camera->viewport_height = GFX_LCD_HEIGHT - tilemap->y_loc;
    camera->max_x = tilemap->width * tilemap->tile_width - camera->viewport_width;
    camera->max_y = tilemap->height * tilemap->tile_height - camera->viewport_height;
}

void choose_level(int level_number, gfx_tilemap_t *tilemap, camera* camera)
{
    struct level *level = levels[level_number];
    player_x = level->start_x;
    player_y = level->start_y;
    level_width = tilemap->width = tilemap->draw_width = level->width;
    level_height = tilemap->height = tilemap->draw_height = level->height;
    if (tilemap->map)
        free(tilemap->map);
    tilemap->map = malloc(level_width * level_height);
    memcpy(tilemap->map, level->map_pointer, level_width * level_height);
    move_count = 0;
    tilemap->x_loc = 0;
    tilemap->y_loc = 16;

    int total_draw_height = level_height * 16;

    if (total_draw_height < GFX_LCD_HEIGHT - 16)
        tilemap->y_loc = GFX_LCD_HEIGHT - total_draw_height;
    camera_reset(camera, tilemap);
    camera_update(camera, 0, player_x * 16, player_y * 16);

    direction = false;
    holding_block = false;
    current_level = level_number;
    move_count = 0;
    level_skip = 0;
    reset = false;
    win_cheat_index = 0;
}

void level_select_start(gfx_tilemap_t *tilemap, camera* camera)
{
    for (max_level = 0; max_level < NUM_BUILTIN_LEVELS; max_level++)
    {
        if (!move_count_list[max_level])
            break;
    }
    if (max_level >= NUM_BUILTIN_LEVELS)
        max_level = NUM_BUILTIN_LEVELS - 1;

    choose_level(max_level, tilemap, camera);
    move_count = move_count_list[current_level];
}

enum state_transition level_select_update(sk_key_t key, gfx_tilemap_t *tilemap, camera* camera)
{
    int level_skip = 0;
    bool exit = false;

    switch (key)
    {
        case sk_Del:
            exit = true;
            break;
        case sk_Left:
            level_skip = -1;
            break;
        case sk_Right:
            level_skip = 1;
            break;
        case sk_2nd:
        case sk_Enter:
            if (current_level <= max_level)
                return st_playing;
            break;
    }

    if (level_skip)
    {
        current_level += level_skip;
        if (current_level < 0)
            current_level = 0;
        else if (current_level > 10)
            current_level = 10;

        choose_level(current_level, tilemap, camera);
        move_count = move_count_list[current_level];

        return st_fade_out;
    }

    camera_update(camera, key, player_x * 16, player_y * 16);

    return exit ? st_back : st_none;
}

bool blocked_at(gfx_tilemap_t *tilemap, int x, int y)
{
    int cell = tilemap->map[y * level_width + x];
    return cell == BLOCK || cell == WALL;
}

void level_select_draw(gfx_tilemap_t *tilemap, camera* camera)
{
    if (current_level <= max_level)
    {
        gfx_Tilemap(tilemap, camera->x, camera->y);
        gfx_SetTextFGColor(8);
        gfx_SetTextScale(1, 1);

        int w = gfx_GetStringWidth("PRESS 2nd TO START");
        gfx_PrintStringXY("PRESS 2nd TO START", (GFX_LCD_WIDTH - w) / 2, 17);
    }
    else
    {
        gfx_SetTextFGColor(8);
        gfx_SetTextScale(3, 3);

        int w = gfx_GetStringWidth("LOCKED");
        gfx_PrintStringXY("LOCKED", (GFX_LCD_WIDTH - w) / 2, (GFX_LCD_HEIGHT - 24) / 2);
    }

    gfx_Sprite_NoClip(arrow_left, 0, 0);
    gfx_Sprite_NoClip(arrow_right, GFX_LCD_WIDTH - 12, 0);

    gfx_SetTextScale(1, 1);
    gfx_SetTextFGColor(5);
    int w = gfx_GetStringWidth("Select Level");
    gfx_PrintStringXY("Select Level", (GFX_LCD_WIDTH - w) / 2, 0);

    snprintf(buffer, 50, "Level %i - Best %u", current_level + 1, move_count);
    w = gfx_GetStringWidth(buffer);
    gfx_PrintStringXY(buffer, (GFX_LCD_WIDTH - w) / 2, 8);
    
    uint24_t draw_x = player_draw_x(tilemap->x_loc);
    uint8_t draw_y = player_draw_y(tilemap->y_loc);

    if (draw_x >= 0 && draw_y >= 16 && draw_x < GFX_LCD_WIDTH && draw_y < GFX_LCD_HEIGHT)
        gfx_ScaledSprite_NoClip(direction ? dude_right : dude, draw_x, draw_y, 2, 2);
}

bool handle_player_movement(sk_key_t key, gfx_tilemap_t *tilemap, camera* camera) {
    int old_x = player_x;
    int old_y = player_y;
    int direction_offset = direction == 0 ? -1 : 1;
    
    switch (key)
    {
        case sk_Mode:
            camera->move_mode = cmm_pan;
            break;
        case sk_Del:
            return true;
        case sk_Left:
            if (direction == 0)
            {
                player_x--;
                if (blocked_at(tilemap, player_x, player_y)
                    && !blocked_at(tilemap, player_x + 1, player_y - 1))
                    player_y--;
            }
            direction = 0;
            break;
        case sk_Right:
            if (direction == 1)
            {
                player_x++;
                if (blocked_at(tilemap, player_x, player_y)
                    && !blocked_at(tilemap, player_x - 1, player_y - 1))
                    player_y--;
            }
            direction = 1;
            break;
        case sk_Up:
            player_x += direction_offset;
            if (blocked_at(tilemap, player_x, player_y)
                && !blocked_at(tilemap, player_x - direction_offset, player_y - 1))
                    player_y--;
            break;
        case sk_2nd:
        case sk_Down:
            if (holding_block)
            {
                int block_x = 0;
                int block_y = 0;

                if (!blocked_at(tilemap, player_x + direction_offset, player_y - 1))
                {
                    block_y = player_y - 1;
                    block_x = player_x + direction_offset;
                    holding_block = false;
                }

                if (!holding_block)
                {
                    while (!blocked_at(tilemap, block_x, block_y + 1))
                        block_y++;

                    tilemap->map[block_y * level_width + block_x] = BLOCK;
                    last_move = drop;
                    undo_x = block_x;
                    undo_y = block_y;
                }
            }
            else
            {
                if (tilemap->map[player_y * level_width + player_x + direction_offset] == BLOCK 
                    && !blocked_at(tilemap, player_x + direction_offset, player_y - 1)
                    && !blocked_at(tilemap, player_x, player_y - 1))
                {
                    tilemap->map[player_y * level_width + player_x + direction_offset] = EMPTY;
                    holding_block = true;
                    last_move = none;
                    undo_x = player_x + direction_offset;
                    undo_y = player_y;
                }
            }
            break;
        case sk_Clear:
            reset = true;
            break;
        case sk_Alpha:
            switch (last_move)
            {
            case move:
                player_x = undo_x;
                player_y = undo_y;
                break;
            case drop:
                holding_block = true;
                tilemap->map[undo_y * level_width + undo_x] = EMPTY;
                move_count++;
            default:
                break;
            }
            last_move = none;
            break;
        }

    if (player_x < 0 || player_x >= level_width || player_y < 0 || player_y >= level_height)
    {
        player_x = old_x;
        player_y = old_y;
    }

    if (blocked_at(tilemap, player_x, player_y))
    {
        player_y = old_y;
        if (blocked_at(tilemap, player_x, player_y))
            player_x = old_x;
    }

    if (holding_block && blocked_at(tilemap, player_x, player_y - 1))
    {
        player_x = old_x;
        player_y = old_y;
    }

    if (player_x != old_x || player_y != old_y)
    {
        move_count++;
        undo_x = old_x;
        undo_y = old_y;
        last_move = move;
    }

    return false;
}

enum state_transition game_update(sk_key_t key, gfx_tilemap_t *tilemap, camera* camera)
{
    if (new_best)
    {
        if (key)
        {
            new_best = false;
            choose_level(current_level + level_skip, tilemap, camera);
            return st_fade_out;
        }
        return st_none;
    }

    level_skip = 0;
    reset = false;
    
    bool exit = false;
    if (camera->move_mode == cmm_pan)
    {
        if (key == sk_Mode || key == sk_Del)
            camera->move_mode = cmm_follow;
    } else {
        exit = handle_player_movement(key, tilemap, camera);
    }
    camera_update(camera, key, player_x * 16, player_y * 16);
    
    int floor = tilemap->map[(player_y + 1) * level_width + player_x];
    while ((floor == EMPTY || floor == DOOR) && player_y < level_height)
    {
        player_y++;
        floor = tilemap->map[(player_y + 1) * level_width + player_x];
    }

    if (!level_skip && tilemap->map[player_y * level_width + player_x] == DOOR)
    {
        level_skip = true;
        if (move_count_list[current_level] == 0 || move_count_list[current_level] > move_count)
        {
            new_best = true;
            move_count_list[current_level] = move_count;
            if (max_level == current_level)
                max_level = current_level + 1;
        }
    }

    if (key != 0)
    {
        win_cheat_index = win_cheat[win_cheat_index] == key ? win_cheat_index + 1 : 0;
        if (win_cheat_index == CHEAT_LENGTH)
        {
            move_count_list[current_level] = UINT16_MAX;
            level_skip = true;
        }
    }

    if (exit)
        return st_back;
    
    if ((level_skip && !new_best) || reset)
    {
        if (current_level + level_skip < NUM_BUILTIN_LEVELS)
        {
            choose_level(current_level + level_skip, tilemap, camera);
            return st_fade_out;
        } else 
            return st_win;
    }

    return st_none;
}

void game_draw(gfx_tilemap_t *tilemap, camera* camera)
{
    gfx_Tilemap(tilemap, camera->x, camera->y);

    gfx_SetTextFGColor(8);
    gfx_SetTextScale(1, 2);
    snprintf(buffer, 50, "Level %i - Moves %u", current_level + 1, move_count);
    gfx_PrintStringXY(buffer, 0, 0);

    int draw_x = player_draw_x(tilemap->x_loc);
    int draw_y = player_draw_y(tilemap->y_loc);

    if (draw_x >= 0 && draw_y >= 16 && draw_x < GFX_LCD_WIDTH && draw_y < GFX_LCD_HEIGHT)
        gfx_ScaledSprite_NoClip(direction ? dude_right : dude, draw_x, draw_y, 2, 2);
    if (holding_block)
    {
        draw_y -= 16;
        if (draw_x >= 0 && draw_y >= 16 && draw_x < GFX_LCD_WIDTH && draw_y < GFX_LCD_HEIGHT)
            gfx_ScaledSprite_NoClip(block, draw_x, draw_y, 2, 2);
    }

    if (camera->move_mode == cmm_pan)
    {
        int half_x = (GFX_LCD_WIDTH - 10) / 2;
        int half_y = (GFX_LCD_HEIGHT - 18) / 2 - 8;

        gfx_TransparentSprite_NoClip(arrow_left, 2, half_y);
        gfx_TransparentSprite_NoClip(arrow_right, GFX_LCD_WIDTH - 10, half_y);
        gfx_TransparentSprite_NoClip(arrow_up, half_x, 18);
        gfx_TransparentSprite_NoClip(arrow_down, half_x, GFX_LCD_HEIGHT - 10);
    }

    if (new_best)
    {
        snprintf(buffer, 50, "NEW BEST!");
        gfx_SetTextFGColor(8);
        gfx_SetTextScale(3, 3);
        int w = gfx_GetStringWidth(buffer);
        gfx_PrintStringXY(buffer, (GFX_LCD_WIDTH - w) / 2, (GFX_LCD_HEIGHT - 24) / 2);
        gfx_SetTextFGColor(5);
        gfx_SetTextScale(1, 1);
        gfx_PrintStringXY("press any key", (GFX_LCD_WIDTH - w) / 2, (GFX_LCD_HEIGHT - 24) / 2 + 26);
    }
}