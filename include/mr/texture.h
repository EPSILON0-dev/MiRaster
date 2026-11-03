#ifndef MR_TEXTURE_H
#define MR_TEXTURE_H

#include "mr/types.h"

struct mr_texture
{
    i32 width, height, channels;
    u8* pixels;
};

#ifdef MR_TEX_FILE_IO
mr_result_t mr_tex_load_file(struct mr_texture* tex, const char* path);
mr_result_t mr_tex_cleanup(struct mr_texture* tex);
#endif

// Note: We assume all textures are tileable and the only correct way to sample them is to repeat
// them
mr_result_t mr_tex_sample_coord(v4u8 color, const v2u16 pos, const struct mr_texture* tex);
mr_result_t mr_tex_sample_nearest(v4u8 color, const v2 uv, const struct mr_texture* tex);

#endif
