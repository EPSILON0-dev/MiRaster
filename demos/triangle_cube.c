#include "mr/camera.h"
#include "mr/math.h"
#include "mr/model.h"
#include "mr/render.h"
#include "mr/shader.h"
#include "mr/types.h"

#define DEGREES_PER_FRAME REAL(100.0f / 30.0f)

// clang-format off
v3 cubeverts[] =
{
    { REAL(-1), REAL(-1), REAL(-1) },
    { REAL( 1), REAL(-1), REAL(-1) },
    { REAL(-1), REAL( 1), REAL(-1) },
    { REAL( 1), REAL( 1), REAL(-1) },
    { REAL(-1), REAL(-1), REAL( 1) },
    { REAL( 1), REAL(-1), REAL( 1) },
    { REAL(-1), REAL( 1), REAL( 1) },
    { REAL( 1), REAL( 1), REAL( 1) },
};

u16 cubeinds[] =
{
    0, 2, 1,  1, 2, 3, // -Z
    4, 5, 6,  5, 7, 6, // +Z
    0, 1, 4,  1, 5, 4, // -Y
    2, 6, 3,  3, 6, 7, // +Y
    0, 4, 2,  2, 4, 6, // -X
    1, 3, 5,  3, 7, 5, // +X
};
// clang-format on

void triangle_shader(v4 color, const struct mr_triangle_shader_inputs* inputs)
{
    v3 pos;
    mr_shader_point_data(pos, inputs, 3);
    v3_maddss(color, pos, REAL(0.45f), REAL(0.60f));
    color[3] = REAL(1);
}

static struct mr_model s_model = {
    .mesh =
        {
            .vertices = (real*)cubeverts,
            .indices = (u16*)cubeinds,
            .stride = sizeof(v3),
            .num_indices = sizeof(cubeinds) / sizeof(*cubeinds),
            .type = MR_MODEL_TRIANGLES,
        },

    .translation = {REAL(0.0f), REAL(0.0f), REAL(2.5f)},
    .rotation = {REAL(0.0f), REAL(0.0f), REAL(0.0f)},
    .scale = {REAL(0.9f), REAL(0.9f), REAL(0.9f)},

    .shader_func = triangle_shader,
    .shader_data = cubeverts,
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
}

void render_loop(struct mr_fb_desc* fb, u32 frame)
{
    const v4u8 clear_color = {0, 0, 0, 255};
    mr_clear_all(fb, clear_color);

    mat4 vp;
    mr_camera_vp_matrix(vp, &s_camera);
    s_model.rotation[1] = real_mul(int2real(frame), MR_DEG2RAD(DEGREES_PER_FRAME));

    mr_model_draw(fb, &s_model, vp);
}
