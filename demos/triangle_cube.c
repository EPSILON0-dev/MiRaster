#include "mr/camera.h"
#include "mr/math.h"
#include "mr/model.h"
#include "mr/render.h"
#include "mr/types.h"

#define DEGREES_PER_SECOND 100.0f

// clang-format off
v3f32 cubeverts[] =
{
    { -1.0f, -1.0f, -1.0f },
    {  1.0f, -1.0f, -1.0f },
    { -1.0f,  1.0f, -1.0f },
    {  1.0f,  1.0f, -1.0f },
    { -1.0f, -1.0f,  1.0f },
    {  1.0f, -1.0f,  1.0f },
    { -1.0f,  1.0f,  1.0f },
    {  1.0f,  1.0f,  1.0f },
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

void triangle_shader(v4f32 color, const struct mr_triangle_shader_inputs* inputs)
{
    v3f32 barycoord;
    mr_triangle_shader_normalize_barycoord(barycoord, inputs);

    v4f32 vertex_color[3];
    for (u8 i = 0; i < 3; i++)
    {
        v3f32_cpy(vertex_color[i], ((v3f32*)inputs->cmd->data)[inputs->cmd->inds[i]]);
        vertex_color[i][3] = 1.0f;
    }

    v4f32_muls(color, vertex_color[0], barycoord[0]);
    v4f32_madds(color, vertex_color[1], barycoord[1], color);
    v4f32_madds(color, vertex_color[2], barycoord[2], color);

    v3f32_muls(color, color, 0.45f);
    v3f32_adds(color, color, 0.60f);
}

static struct mr_model s_model = {
    .mesh =
        {
            .vertices = (f32*)cubeverts,
            .indices = (u16*)cubeinds,
            .stride = sizeof(v3f32),
            .num_indices = sizeof(cubeinds) / sizeof(*cubeinds),
            .type = MR_MODEL_TRIANGLES,
        },

    .translation = {0.0f, 0.0f, 2.5f},
    .rotation = {0.0f, 0.0f, 0.0f},
    .scale = {0.9f, 0.9f, 0.9f},

    .shader_func = triangle_shader,
    .shader_data = cubeverts,
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
    mr_clear_all(fb, clear_color);

    mat4f32 vp;
    mr_camera_vp_matrix(vp, &s_camera);
    s_model.rotation[1] = ((f32)frame) / 30.0f * MR_DEG2RAD(DEGREES_PER_SECOND);

    mr_model_draw(fb, &s_model, vp);
}