#include "mr/math.h"
#include "mr/render.h"

// clang-format off
v4f32 cube_vertices[] =
{
    { -1.0f, -1.0f, -1.0f, 1.0f },
    {  1.0f, -1.0f, -1.0f, 1.0f },
    { -1.0f,  1.0f, -1.0f, 1.0f },
    {  1.0f,  1.0f, -1.0f, 1.0f },
    { -1.0f, -1.0f,  1.0f, 1.0f },
    {  1.0f, -1.0f,  1.0f, 1.0f },
    { -1.0f,  1.0f,  1.0f, 1.0f },
    {  1.0f,  1.0f,  1.0f, 1.0f },
};

u32 cube_indices[] =
{
    0, 2, 1,  1, 2, 3, // -Z
    4, 5, 6,  5, 7, 6, // +Z
    0, 1, 4,  1, 5, 4, // -Y
    2, 6, 3,  3, 6, 7, // +Y
    0, 4, 2,  2, 4, 6, // -X
    1, 3, 5,  3, 7, 5, // +X
};
// clang-format on

void tri_shader(v4f32 color, const struct mr_triangle_shader_inputs* inputs)
{
    v4f32_cpy(color, inputs->cmd->data);
}

void render_image(struct fb_desc* fb, u32 frame)
{
    const v4u8 clear_color = {0, 0, 0, 255};
    mr_clear_all(fb, clear_color);

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
        1.0f / aspect, 0.0f,  0.0f,       0.0f,
        0.0f,          1.0f,  0.0f,       0.0f,
        0.0f,          0.0f,  1.002002f,  1.0f,
        0.0f,          0.0f,  0.200200f,  0.0f
    };
    // clang-format on

    mat4f32 transform_mat;
    mat4f32_mulmat(transform_mat, scale_mat, rotate_mat);
    mat4f32_mulmat(transform_mat, translate_mat, transform_mat);
    mat4f32_mulmat(transform_mat, perspective_mat, transform_mat);

    v4f32 tri[3];
    struct mr_triangle_draw_cmd cmd = {
        .fb = fb,
        .verts = tri,
        .shader = tri_shader,
        .data = NULL,
    };

    for (size_t i = 0; i < sizeof(cube_indices) / sizeof(*cube_indices); i += 3)
    {
        for (size_t j = 0; j < 3; j++)
        {
            mat4f32_mulvec(tri[j], transform_mat, cube_vertices[cube_indices[i + j]]);
            f32 w = tri[j][3];
            tri[j][3] = 1.0f;
            v4f32_divs(tri[j], tri[j], f32_abs(w));
        }

        const u16 tri_index = ((i / 3) % 7) + 1;
        v4f32 color = {
            (tri_index & 1) ? 1.0f : 0.0f,  //
            (tri_index & 2) ? 1.0f : 0.0f,  //
            (tri_index & 4) ? 1.0f : 0.0f,  //
            1.0f                            //
        };
        cmd.data = color;
        mr_draw_triangle(&cmd);
    }
}