#include "mr/math.h"
#include "mr/render.h"

// clang-format off
v4f32 cubebase[] =
{
    { -1.0f, -1.0f, -1.0f, 1.0f }, {  1.0f, -1.0f, -1.0f, 1.0f },
    { -1.0f,  1.0f, -1.0f, 1.0f }, {  1.0f,  1.0f, -1.0f, 1.0f },
    { -1.0f, -1.0f,  1.0f, 1.0f }, {  1.0f, -1.0f,  1.0f, 1.0f },
    { -1.0f,  1.0f,  1.0f, 1.0f }, {  1.0f,  1.0f,  1.0f, 1.0f },

    { -1.0f, -1.0f, -1.0f, 1.0f }, { -1.0f,  1.0f, -1.0f, 1.0f },
    {  1.0f, -1.0f, -1.0f, 1.0f }, {  1.0f,  1.0f, -1.0f, 1.0f },
    { -1.0f, -1.0f,  1.0f, 1.0f }, { -1.0f,  1.0f,  1.0f, 1.0f },
    {  1.0f, -1.0f,  1.0f, 1.0f }, {  1.0f,  1.0f,  1.0f, 1.0f },

    { -1.0f, -1.0f, -1.0f, 1.0f }, { -1.0f, -1.0f,  1.0f, 1.0f },
    {  1.0f, -1.0f, -1.0f, 1.0f }, {  1.0f, -1.0f,  1.0f, 1.0f },
    { -1.0f,  1.0f, -1.0f, 1.0f }, { -1.0f,  1.0f,  1.0f, 1.0f },
    {  1.0f,  1.0f, -1.0f, 1.0f }, {  1.0f,  1.0f,  1.0f, 1.0f },
};
// clang-format on

void tri_shader(v4f32 color, v2u16 screenpos, const struct mr_line_draw_cmd* cmd)
{
    v4f32 pos;
    f32 factor;
    mr_fbcoord_to_line_pos(pos, &factor, cmd->fb, screenpos, cmd->verts);

    const f32 brightness = f32_clamp(pos[3], 0.0f, 1.0f);
    color[0] = brightness;
    color[1] = (factor * brightness) * 0.5f + 0.5f;
    color[2] = ((1.0f - factor) * brightness) * 0.5f + 0.5f;
    color[3] = 1.0f;
}

void render_image(struct fb_desc* fb, u32 frame)
{
    const v4u8 clear_color = {0, 0, 0, 255};
    mr_clear_color(fb, clear_color);

    // Note: Since we store matrices column by column their deifinitions have to be transposed

    mat4f32 scale_mat = {
        0.9f, 0.0f, 0.0f, 0.0f,  //
        0.0f, 0.9f, 0.0f, 0.0f,  //
        0.0f, 0.0f, 0.9f, 0.0f,  //
        0.0f, 0.0f, 0.0f, 1.0f   //
    };

    // clang-format off
    const f32 phi = ((f32)frame) / 100.0f;
    const mat4f32 rotate_mat = {
         cosf(phi), 0.0f, sinf(phi), 0.0f,
              0.0f, 1.0f,      0.0f, 0.0f,
        -sinf(phi), 0.0f, cosf(phi), 0.0f,
              0.0f, 0.0f,      0.0f, 1.0f
    };
    // clang-format on

    const mat4f32 translate_mat = {
        1.0f, 0.0f, 0.0f, 0.0f,  //
        0.0f, 1.0f, 0.0f, 0.0f,  //
        0.0f, 0.0f, 1.0f, 0.0f,  //
        0.0f, 0.0f, 2.5f, 1.0f   //
    };

    // clang-format off
    const f32 aspect = (f32)fb->width / (f32)fb->height;
    const mat4f32 perspective_mat = {
        1.0f / aspect, 0.0f, 0.0f, 0.0f,
        0.0f,          1.0f, 0.0f, 0.0f,
        0.0f,          0.0f, 1.0f, 1.0f,
        0.0f,          0.0f, 0.0f, 0.0f
    };
    // clang-format on

    mat4f32 transform_mat;
    mat4f32_mulmat(transform_mat, scale_mat, rotate_mat);
    mat4f32_mulmat(transform_mat, translate_mat, transform_mat);
    mat4f32_mulmat(transform_mat, perspective_mat, transform_mat);

    v4f32 line[2];
    struct mr_line_draw_cmd cmd = {
        .fb = fb,
        .verts = line,
        .shader = tri_shader,
        .data = NULL,
    };

    for (size_t i = 0; i < sizeof(cubebase) / sizeof(*cubebase); i += 2)
    {
        for (size_t j = 0; j < 2; j++)
        {
            mat4f32_mulvec(line[j], transform_mat, cubebase[i + j]);
            f32 w = line[j][3];
            line[j][3] = 1.0f;
            v4f32_divs(line[j], line[j], f32_abs(w));
        }

        mr_draw_line(&cmd);
    }
}
