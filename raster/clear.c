#include <float.h>

#include "mr/render.h"
#include "mr/types.h"

mr_result_t mr_clear_color(struct mr_fb_desc* fb, const v4u8 color)
{
    const u32 color32 = *((u32*)color);
    for (size_t i = 0; i < fb->width * fb->height; i++)
    {
        ((u32*)fb->pixels)[i] = color32;
    }
    return MR_OK;
}

mr_result_t mr_clear_depth(struct mr_fb_desc* fb)
{
    for (size_t i = 0; i < fb->width * fb->height; i++)
    {
        ((f32*)fb->depth)[i] = 0;
    }
    return MR_OK;
}

mr_result_t mr_clear_all(struct mr_fb_desc* fb, const v4u8 color)
{
    mr_clear_color(fb, color);
    mr_clear_depth(fb);
    return MR_OK;
}