#include <stdint.h>
#include <stdbool.h>
#include <graphx.h>

#include "camera.h"

camera main_camera;

void cmm_pan_update(sk_key_t key, camera* camera) {
    switch (key)
    {
        case sk_Left:
            if (camera->x > camera->min_x || camera->bounary_mode == cbm_loose)
                camera->x -= camera->step_x;
            break;
        case sk_Right:
            if (camera->x < camera->max_x || camera->bounary_mode == cbm_loose)
                camera->x += camera->step_x;
            break;
        case sk_Up:
            if (camera->y > camera->min_y || camera->bounary_mode == cbm_loose)
                camera->y -= camera->step_y;
            break;
        case sk_Down:
            if (camera->y < camera->max_y || camera->bounary_mode == cbm_loose)
                camera->y += camera->step_y;
            break;
    }
}

void cmm_follow_update(camera* camera, uint24_t followX, uint24_t followY) {
    camera->x = followX - camera->viewport_width / 2;
    camera->y = followY - camera->viewport_height / 2;
}

void camera_update(camera* camera, sk_key_t key, int24_t followX, int24_t followY) {
    switch (camera->move_mode) {
        case cmm_follow:
            cmm_follow_update(camera, followX, followY);
            break;
        case cmm_pan:
            cmm_pan_update(key, camera);
            break;
    }

    if (camera->bounary_mode == cbm_strict) {
        if (camera->x < camera->min_x)
            camera->x = camera->min_x;
        else if (camera->x > camera->max_x)
            camera->x = camera->max_x;

        if (camera->y < camera->min_y)
            camera->y = camera->min_y;
        else if (camera->y > camera->max_y)
            camera->y = camera->max_y;
    }
}