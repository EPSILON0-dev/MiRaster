#include <stdio.h>

#include "mr/camera.h"
#include "mr/math.h"
#include "mr/model.h"
#include "mr/render.h"
#include "mr/shader.h"
#include "mr/texture.h"
#include "mr/types.h"

#define DEGREES_PER_FRAME REAL(100.0f / 30.0f)

#define R(x) REAL(x)

struct vertex
{
    v3 pos;
    v3 norm;
    v2 uv;
};

// clang-format off
static const struct vertex cubeverts[] =
{
      //    Position     //   //     Normal      //   //   UV   //
    { {R(-1), R(-1), R(-1)},  {R( 1), R( 1), R(-1)},  {R(0), R(0)} }, // -Z
    { {R( 1), R(-1), R(-1)},  {R( 1), R( 1), R(-1)},  {R(1), R(0)} }, //
    { {R(-1), R( 1), R(-1)},  {R( 1), R( 1), R(-1)},  {R(0), R(1)} }, //
    { {R( 1), R( 1), R(-1)},  {R( 1), R( 1), R(-1)},  {R(1), R(1)} }, //
    { {R(-1), R(-1), R( 1)},  {R( 1), R( 1), R( 1)},  {R(0), R(0)} }, // +Z
    { {R( 1), R(-1), R( 1)},  {R( 1), R( 1), R( 1)},  {R(1), R(0)} }, //
    { {R(-1), R( 1), R( 1)},  {R( 1), R( 1), R( 1)},  {R(0), R(1)} }, //
    { {R( 1), R( 1), R( 1)},  {R( 1), R( 1), R( 1)},  {R(1), R(1)} }, //
    { {R(-1), R(-1), R(-1)},  {R( 1), R(-1), R( 1)},  {R(0), R(0)} }, // -Y
    { {R( 1), R(-1), R(-1)},  {R( 1), R(-1), R( 1)},  {R(1), R(0)} }, //
    { {R(-1), R(-1), R( 1)},  {R( 1), R(-1), R( 1)},  {R(0), R(1)} }, //
    { {R( 1), R(-1), R( 1)},  {R( 1), R(-1), R( 1)},  {R(1), R(1)} }, //
    { {R(-1), R( 1), R(-1)},  {R( 1), R( 1), R( 1)},  {R(0), R(0)} }, // +Y
    { {R( 1), R( 1), R(-1)},  {R( 1), R( 1), R( 1)},  {R(1), R(0)} }, //
    { {R(-1), R( 1), R( 1)},  {R( 1), R( 1), R( 1)},  {R(0), R(1)} }, //
    { {R( 1), R( 1), R( 1)},  {R( 1), R( 1), R( 1)},  {R(1), R(1)} }, //
    { {R(-1), R(-1), R(-1)},  {R(-1), R( 1), R( 1)},  {R(0), R(0)} }, // -X
    { {R(-1), R(-1), R( 1)},  {R(-1), R( 1), R( 1)},  {R(1), R(0)} }, //
    { {R(-1), R( 1), R(-1)},  {R(-1), R( 1), R( 1)},  {R(0), R(1)} }, //
    { {R(-1), R( 1), R( 1)},  {R(-1), R( 1), R( 1)},  {R(1), R(1)} }, //
    { {R( 1), R(-1), R(-1)},  {R( 1), R( 1), R( 1)},  {R(0), R(0)} }, // +X
    { {R( 1), R(-1), R( 1)},  {R( 1), R( 1), R( 1)},  {R(1), R(0)} }, //
    { {R( 1), R( 1), R(-1)},  {R( 1), R( 1), R( 1)},  {R(0), R(1)} }, //
    { {R( 1), R( 1), R( 1)},  {R( 1), R( 1), R( 1)},  {R(1), R(1)} }, //
};

u16 cubeinds[] =
{
     0,  2,  1,   1,  2,  3, // -Z
     4,  5,  6,   5,  7,  6, // +Z
     8,  9, 10,   9, 11, 10, // -Y
    12, 14, 13,  13, 14, 15, // +Y
    16, 17, 18,  17, 19, 18, // -X
    20, 22, 21,  21, 22, 23, // +X
};
// clang-format on

struct texture
{
    i32 w, h, c;
    u8* pixels;
};

static struct mr_texture s_tex_albedo;
static struct mr_texture s_tex_normal;

void cube_shader(v4 color, const struct mr_triangle_shader_inputs* inputs)
{
    static struct vertex vert;
    mr_shader_point_data(&vert, inputs, 8);

    v4u8 ucolor;
    mr_tex_sample_nearest(ucolor, vert.uv, &s_tex_albedo);
    mr_color_u8_to_real(color, ucolor);
}

void ground_shader(v4 color, const struct mr_triangle_shader_inputs* inputs)
{
    (void)inputs;
    for (u8 i = 0; i < 3; i++) color[i] = REAL(0.6f);
    color[3] = REAL(1);
}

static struct mr_model s_cube = {
    .mesh =
        {
            .vertices = (real*)cubeverts,
            .indices = (u16*)cubeinds,
            .stride = sizeof(struct vertex),
            .num_indices = sizeof(cubeinds) / sizeof(*cubeinds),
            .type = MR_MODEL_TRIANGLES,
        },

    .translation = {REAL(0.0f), REAL(0.0f), REAL(2.5f)},
    .rotation = {REAL(0.0f), REAL(0.0f), REAL(0.0f)},
    .scale = {REAL(0.9f), REAL(0.9f), REAL(0.9f)},

    .shader_func = cube_shader,
};

static struct mr_model s_ground = {
    .mesh =
        {
            .vertices = (real*)cubeverts,
            .indices = &((u16*)(cubeinds))[3 * 6],  // 4th face is the top face
            .stride = sizeof(struct vertex),
            .num_indices = 6,
            .type = MR_MODEL_TRIANGLES,
        },

    .translation = {REAL(0.0f), REAL(-2), REAL(2.5f)},
    .rotation = {REAL(0.0f), REAL(0.0f), REAL(0.0f)},
    .scale = {REAL(3), REAL(1), REAL(3)},

    .shader_func = ground_shader,
    .shader_data = NULL,
};

static struct mr_camera s_camera = {
    .pos = {REAL(0), REAL(1.25f), REAL(0)},
    .euler = {REAL(30.0f / 180.0f * M_PI), REAL(0), REAL(0)},
    .aspect = REAL(0),  // Set in the setup
    .yfovdegrees = REAL(90),
    .near = REAL(0.01f),
    .far = REAL(1000),
};

void render_setup(struct mr_fb_desc* fb)
{
    s_camera.aspect = real_div(int2real(fb->width), int2real(fb->height));

    if (mr_tex_load_file(&s_tex_albedo, "./assets/bricks/albedo.png") != MR_OK)
    {
        fprintf(stderr, "Failed to load the albedo bricks texture\n");
    }

    if (mr_tex_load_file(&s_tex_normal, "./assets/bricks/normal.png") != MR_OK)
    {
        fprintf(stderr, "Failed to load the normal bricks texture\n");
    }
}

void render_loop(struct mr_fb_desc* fb, u32 frame)
{
    const v4u8 clear_color = {0, 0, 0, 255};
    mr_clear_all(fb, clear_color);

    mat4 vp;
    mr_camera_vp_matrix(vp, &s_camera);
    s_cube.rotation[1] = real_mul(int2real(frame), MR_DEG2RAD(DEGREES_PER_FRAME));
    // mr_model_matrix(cube_shader_data.model_matrix, &s_cube);

    mr_model_draw(fb, &s_cube, vp);
    mr_model_draw(fb, &s_ground, vp);
}
