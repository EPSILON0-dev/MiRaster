#include "mr/texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "mr/math.h"  // IWYU pragma: keep

#ifdef MR_TEX_FILE_IO
mr_result_t mr_tex_load_file(struct mr_texture* tex, const char* path)
{
    tex->pixels = stbi_load(path, &tex->width, &tex->height, &tex->channels, 4);
    return (tex->pixels) ? MR_OK : MR_LOAD_FAILED;
}

mr_result_t mr_tex_cleanup(struct mr_texture* tex)
{
    if (tex->pixels) STBI_FREE(tex->pixels);
    return MR_OK;
}
#endif

mr_result_t mr_tex_sample_coord(v4u8 color, const v2u16 pos, const struct mr_texture* tex)
{
    if (pos[0] >= tex->width || pos[1] >= tex->height) return MR_OUT_OF_RANGE;
    const size_t idx = (pos[1] * tex->width + pos[0]) * 4;
    v4u8_cpy(color, &tex->pixels[idx]);
    return MR_OK;
}

mr_result_t mr_tex_sample_nearest(v4u8 color, const v2 uv, const struct mr_texture* tex)
{
    const v2 real_uv = {real_fract(uv[0]), real_fract(uv[1])};
    const v2u16 pos = {
        real2int(real_uv[0] * (tex->width - 1)),  //
        real2int(real_uv[1] * (tex->height - 1))  //
    };
    mr_tex_sample_coord(color, pos, tex);
    return MR_OK;
}
