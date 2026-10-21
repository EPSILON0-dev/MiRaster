#include "mr/fb.h"

void mr_fbcoord_to_fragcoord(struct fb_desc* fb, v2f32 fragcoord, v2u16 fbcoord)
{
    fragcoord[0] = ((f32)fbcoord[0]) / (fb->width - 1) * 2.0f - 1.0f;
    fragcoord[1] = ((f32)fbcoord[1]) / (fb->height - 1) * 2.0f - 1.0f;
}

void mr_fragcoord_to_fbcoord(struct fb_desc* fb, v2u16 fbcoord, v2f32 fragcoord)
{
    fbcoord[0] = (u16)((fragcoord[0] * 0.5f + 0.5f) * (fb->width - 1));
    fbcoord[1] = (u16)((fragcoord[1] * 0.5f + 0.5f) * (fb->height - 1));
}
