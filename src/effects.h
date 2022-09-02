#ifndef __EFFECTS_H_
#define __EFFECTS_H_

#include <graphx.h>

void fade_out(uint16_t* base_palette, uint8_t start, uint8_t length)
{
    for (int step = 255; step >= 0; step--)
    {
        for (int i = start; i < start + length; i++)
            gfx_palette[i] = gfx_Darken(base_palette[i], step);
    }
}

void fade_in(uint16_t* base_palette, uint8_t start, uint8_t length)
{
    for (int step = 0; step < 256; step++)
    {
        for (int i = start; i < start + length; i++)
            gfx_palette[i] = gfx_Darken(base_palette[i], step);
    }
}

uint16_t HsvToRgb(uint8_t h, uint8_t s, uint8_t v)
{
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    if (s == 0)
    {
        r = v;
        g = v;
        b = v;
        return gfx_RGBTo1555(r, g, b);
    }

    uint8_t region = h / 43;
    uint8_t remainder = (h - (region * 43)) * 6; 

    uint8_t p = (v * (255 - s)) >> 8;
    uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
        case 0:
            r = v; g = t; b = p;
            break;
        case 1:
            r = q; g = v; b = p;
            break;
        case 2:
            r = p; g = v; b = t;
            break;
        case 3:
            r = p; g = q; b = v;
            break;
        case 4:
            r = t; g = p; b = v;
            break;
        default:
            r = v; g = p; b = q;
            break;
    }

    return gfx_RGBTo1555(r, g, b);
}

#endif