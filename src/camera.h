#ifndef __CAMERA_H_
#define __CAMERA_H_

#include <ti/getcsc.h>

typedef enum { cmm_follow, cmm_pan } camera_move_mode;
typedef enum { cbm_strict, cbm_loose } camera_boundary_mode;

typedef struct {
    int24_t x, y;
    int24_t min_x, min_y;
    int24_t max_x, max_y;
    int24_t step_x, step_y;
    camera_move_mode move_mode;
    camera_boundary_mode bounary_mode;
    uint24_t viewport_width, viewport_height;
} camera;
extern camera main_camera;

void camera_update(camera* camera, sk_key_t key, int24_t followX, int24_t followY);

#endif