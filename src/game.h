#ifndef __GAME_H_
#define __GAME_H_

#include "main.h"

enum state_transition game_update(sk_key_t key, gfx_tilemap_t *tilemap);
void game_draw(gfx_tilemap_t *tilemap);

void level_select_start(gfx_tilemap_t *tilemap);
enum state_transition level_select_update(sk_key_t key, gfx_tilemap_t *tilemap);
void level_select_draw(gfx_tilemap_t *tilemap);

#endif