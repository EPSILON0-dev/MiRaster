#include "mr/render.h"
#include "mr/types.h"

mr_result_t mr_clear(struct fb_desc* desc, const v4u8 color)
{
    const u32 color32 = *((u32*)color);
    for (size_t i = 0; i < desc->width * desc->height; i++)
    {
        ((u32*)desc->pixels)[i] = color32;
    }
    return MR_OK;
}
