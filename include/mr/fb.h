#ifndef MR_FB_H
#define MR_FB_H

#include "mr/config.h"
#include "mr/types.h"

struct fb_desc
{
    u16 width;
    u16 height;
    u8 channels;

    struct
    {
        u8 pixel_type_uint8 : 1;
        u8 pixel_format_rgba : 1;
    } flags;

    // Must be 64-byte aligned
    void* pixels;
    void* depth;
};

MR_FORCE_STATIC_INLINE mr_result_t mr_fb_draw_pixel(struct fb_desc* fb, v2u16 pos, v4u8 color)
{
    if (pos[0] >= fb->width || pos[1] >= fb->height)
    {
        return MR_OUT_OF_RANGE;
    }

    const size_t idx = pos[1] * fb->width + pos[0];
    ((u32*)fb->pixels)[idx] = *((u32*)color);

    return MR_OK;
}

MR_FORCE_STATIC_INLINE mr_result_t mr_fb_draw_depth(struct fb_desc* fb, v2u16 pos, f32 depth)
{
    if (pos[0] >= fb->width || pos[1] >= fb->height)
    {
        return MR_OUT_OF_RANGE;
    }

    const size_t idx = pos[1] * fb->width + pos[0];
    ((f32*)fb->depth)[idx] = depth;

    return MR_OK;
}

MR_FORCE_STATIC_INLINE float mr_fb_get_depth(struct fb_desc* fb, v2u16 pos)
{
    if (pos[0] >= fb->width || pos[1] >= fb->height)
    {
        return MR_OUT_OF_RANGE;
    }

    const size_t idx = pos[1] * fb->width + pos[0];
    return ((u32*)fb->depth)[idx];
}

#endif
