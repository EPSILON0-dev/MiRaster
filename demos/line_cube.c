#include "mr/camera.h"
#include "mr/math.h"
#include "mr/model.h"
#include "mr/render.h"

#define DEGREES_PER_SECOND 100.0f

// clang-format off
static const v3f32 cubeverts[] =
{
    { -1.0f, -1.0f, -1.0f },  // 0
    {  1.0f, -1.0f, -1.0f },  // 1
    { -1.0f,  1.0f, -1.0f },  // 2
    {  1.0f,  1.0f, -1.0f },  // 3
    { -1.0f, -1.0f,  1.0f },  // 4
    {  1.0f, -1.0f,  1.0f },  // 5
    { -1.0f,  1.0f,  1.0f },  // 6
    {  1.0f,  1.0f,  1.0f },  // 7
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

void line_shader(v4f32 color, v2u16 screenpos, const struct mr_line_draw_cmd* cmd)
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

static struct mr_model s_model = {
    .mesh =
        {
            .vertices = (f32*)cubeverts,
            .indices = (u16*)cubeinds,
            .stride = sizeof(v3f32),
            .num_indices = sizeof(cubeinds) / sizeof(*cubeinds),
            .type = MR_MODEL_LINES,
        },

    .translation = {0.0f, 0.0f, 2.5f},
    .rotation = {0.0f, 0.0f, 0.0f},
    .scale = {0.9f, 0.9f, 0.9f},

    .shader_func = line_shader,
    .shader_data = NULL,
};

static struct mr_camera s_camera = {
    .pos = {0.0f, 0.0f, 0.0f},
    .euler = {0.0f, 0.0f, 0.0f},
    .aspect = 0.0f,  // Set in the setup
    .yfovdegrees = 90.0f,
    .near = 0.01f,
    .far = 1000.f,
};

void render_setup(struct mr_fb_desc* fb)
{
    s_camera.aspect = (f32)fb->width / (f32)fb->height;
}

void render_loop(struct mr_fb_desc* fb, u32 frame)
{
    const v4u8 clear_color = {0, 0, 0, 255};
    mr_clear_color(fb, clear_color);

    mat4f32 vp;
    mr_camera_vp_matrix(vp, &s_camera);
    s_model.rotation[1] = ((f32)frame) / 30.0f * MR_DEG2RAD(DEGREES_PER_SECOND);

    mr_model_draw(fb, &s_model, vp);
}
