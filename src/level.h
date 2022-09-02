#ifndef __LEVEL_H_
#define __LEVEL_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct level
{
    uint8_t width, height;
    uint8_t start_x, start_y;
    uint8_t* map_pointer;
};

#define NUM_BUILTIN_LEVELS 11
extern struct level** levels;
extern uint16_t* move_count_list;

extern struct level title_screen;

void levels_load();
void levels_free();

#endif