#include "mr/camera.h"
#include "mr/math.h"
#include "mr/model.h"
#include "mr/render.h"

#define DEGREES_PER_FRAME REAL(100.0f / 30.0f)

// clang-format off
static const v3 cubeverts[] =
{
    { REAL(-1), REAL(-1), REAL(-1) },  // 0
    { REAL( 1), REAL(-1), REAL(-1) },  // 1
    { REAL(-1), REAL( 1), REAL(-1) },  // 2
    { REAL( 1), REAL( 1), REAL(-1) },  // 3
    { REAL(-1), REAL(-1), REAL( 1) },  // 4
    { REAL( 1), REAL(-1), REAL( 1) },  // 5
    { REAL(-1), REAL( 1), REAL( 1) },  // 6
    { REAL( 1), REAL( 1), REAL( 1) },  // 7
};

static const u16 cubeinds[] =
{
    0, 1,  // Bottom-back edge
    2, 3,  // Top-back edge
    4, 5,  // Bottom-front edge
    6, 7,  // Top-front edge

    0, 2,  // Left-back edge
    1, 3,  // Right-back edge
    4, 6,  // Left-front edge
    5, 7,  // Right-front edge

    0, 4,  // Bottom-left edge
    1, 5,  // Bottom-right edge
    2, 6,  // Top-left edge
    3, 7,  // Top-right edge
};
// clang-format on

void line_shader(v4 color, v2u16 screenpos, const struct mr_line_draw_cmd* cmd)
{
    v4 pos;
    real factor;
    mr_fbcoord_to_line_pos(pos, &factor, cmd->fb, screenpos, cmd->verts);

    const real brightness = real_clamp(pos[3], REAL(0), REAL(1));
    color[0] = brightness;
    color[1] = real_add(real_mul(real_mul(factor, brightness), REAL(0.5f)), REAL(0.5f));
    color[2] =
        real_add(real_mul(real_mul(real_sub(REAL(1), factor), brightness), REAL(0.5f)), REAL(0.5f));
    color[3] = REAL(1);
}

static struct mr_model s_model = {
    .mesh =
        {
            .vertices = (real*)cubeverts,
            .indices = (u16*)cubeinds,
            .stride = sizeof(v3),
            .num_indices = sizeof(cubeinds) / sizeof(*cubeinds),
            .type = MR_MODEL_LINES,
        },

    .translation = {REAL(0.0f), REAL(0.0f), REAL(2.5f)},
    .rotation = {REAL(0.0f), REAL(0.0f), REAL(0.0f)},
    .scale = {REAL(0.9f), REAL(0.9f), REAL(0.9f)},

    .shader_func = line_shader,
    .shader_data = NULL,
};

static struct mr_camera s_camera = {
    .pos = {REAL(0), REAL(0), REAL(0)},
    .euler = {REAL(0), REAL(0), REAL(0)},
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
    mr_clear_color(fb, clear_color);

    mat4 vp;
    mr_camera_vp_matrix(vp, &s_camera);
    s_model.rotation[1] = real_mul(int2real(frame), MR_DEG2RAD(DEGREES_PER_FRAME));

    mr_model_draw(fb, &s_model, vp);
}
