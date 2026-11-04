#ifndef MR_FB_H
#define MR_FB_H

#include "mr/config.h"
#include "mr/types.h"

struct mr_fb_desc
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

MR_INLINE mr_result_t mr_fb_draw_pixel(struct mr_fb_desc* fb, v2u16 pos, v4u8 color)
{
    if (pos[0] >= fb->width || pos[1] >= fb->height)
    {
        return MR_OUT_OF_RANGE;
    }

    const size_t idx = pos[1] * fb->width + pos[0];
    ((u32*)fb->pixels)[idx] = *((u32*)color);

    return MR_OK;
}

MR_INLINE mr_result_t mr_fb_draw_depth(struct mr_fb_desc* fb, v2u16 pos, real depth)
{
    if (pos[0] >= fb->width || pos[1] >= fb->height)
    {
        return MR_OUT_OF_RANGE;
    }

    const size_t idx = pos[1] * fb->width + pos[0];
    ((real*)fb->depth)[idx] = depth;

    return MR_OK;
}

MR_INLINE real mr_fb_get_depth(struct mr_fb_desc* fb, v2u16 pos)
{
    if (pos[0] >= fb->width || pos[1] >= fb->height)
    {
        return MR_OUT_OF_RANGE;
    }

    const size_t idx = pos[1] * fb->width + pos[0];
    return ((real*)fb->depth)[idx];
}

#endif
