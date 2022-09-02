#ifndef __MENU_H_
#define __MENU_H_

#include <ti/getcsc.h>
#include <graphx.h>

void menu_load(gfx_tilemap_t* tilemap);
enum state_transition main_menu_update(sk_key_t key);
void main_menu_draw(gfx_tilemap_t* tilemap);

#endif