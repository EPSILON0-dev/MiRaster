#include <float.h>
#include <stdlib.h>

#include "mr/fb.h"
#include "mr/math.h"
#include "mr/render.h"
#include "mr/types.h"

struct triangle_edges
{
    u16 left;
    u16 right;
};

static struct
{
    struct triangle_edges* edge_buff;
    u16 fb_height;
} s_triange_renderer_store;

MR_FORCE_STATIC_INLINE mr_result_t allocate_edge_buff(struct mr_fb_desc* fb)
{
    if (s_triange_renderer_store.edge_buff)
    {
        free(s_triange_renderer_store.edge_buff);
    }

    s_triange_renderer_store.edge_buff = malloc(sizeof(struct triangle_edges) * fb->height);
    s_triange_renderer_store.fb_height = fb->height;

    if (!s_triange_renderer_store.edge_buff)
    {
        return MR_NOMEM;
    }

    return MR_OK;
}

static void draw_triangle_pixel(v2u16 pos, __mr_internal_line_pixel_data* data)
{
    // Compute the coords within a triangle
    struct mr_triangle_draw_cmd* cmd = data;
    struct mr_triangle_shader_inputs inputs;
    inputs.cmd = cmd;
    mr_fbcoord_to_triangle_pos(inputs.pos, inputs.barycoord, cmd->fb, pos, cmd->verts);

    // Perform a depth test
    const f32 depth = mr_fb_get_depth(cmd->fb, pos);
    if (inputs.pos[3] <= depth) return;

    // Compute the pixel color
    v4f32 fcolor;
    v4u8 ucolor;
    cmd->shader(fcolor, &inputs);
    mr_color_f32_to_u8(ucolor, fcolor);
    mr_fb_draw_pixel(cmd->fb, pos, ucolor);

    // Store the new depth at this position
    mr_fb_draw_depth(cmd->fb, pos, inputs.pos[3]);
}

static void draw_line_pixel_func(v2u16 pos, __mr_internal_line_pixel_data* data)
{
    // Store the horizontal extents
    struct triangle_edges* edges = &s_triange_renderer_store.edge_buff[pos[1]];
    if (pos[0] < edges->left) edges->left = pos[0];
    if (pos[0] > edges->right) edges->right = pos[0];
    draw_triangle_pixel(pos, data);
}

mr_result_t mr_draw_triangle(struct mr_triangle_draw_cmd* cmd)
{
    // Buffer allocation guard
    if (s_triange_renderer_store.edge_buff == NULL ||
        s_triange_renderer_store.fb_height != cmd->fb->height)
    {
        mr_result_t res = allocate_edge_buff(cmd->fb);
        if (res != MR_OK) return res;
    }
    struct triangle_edges* triedges = s_triange_renderer_store.edge_buff;

    // Compute the vertical bounds for the triangle
    u16 min = cmd->fb->height - 1;
    u16 max = 0;
    for (u8 i = 0; i < 3; i++)
    {
        v2f32 fpos;
        v2f32_cpy(fpos, cmd->verts[i]);
        v2f32_clamp(fpos, -1.0f, 1.0f);

        v2u16 pos;
        mr_fragcoord_to_fbcoord(cmd->fb, pos, fpos);

        if (pos[1] < min) min = pos[1];
        if (pos[1] > max) max = pos[1];
    }

    // Clean the edge buffer's vertical bounds
    for (u16 i = min; i <= max; i++)
    {
        triedges[i].left = cmd->fb->width - 1;
        triedges[i].right = 0;
    }

    // Draw the outer lines
    v4f32 verts[3];
    struct mr_line_draw_cmd linecmd = {
        .verts = verts,
        .fb = cmd->fb,
        .shader = NULL,
        .data = NULL,
    };

    for (u8 i = 0; i < 3; i++)
    {
        v4f32_cpy(verts[0], cmd->verts[i]);
        v4f32_cpy(verts[1], cmd->verts[(i + 1) % 3]);
        __mr_internal_draw_line(&linecmd, draw_line_pixel_func, cmd);
    }

    // Fill the lines within the triangle
    for (u16 y = min; y <= max; y++)
    {
        struct triangle_edges* edges = &s_triange_renderer_store.edge_buff[y];
        for (u16 x = edges->left; x <= edges->right; x++)
        {
            v2u16 pos = {x, y};
            draw_triangle_pixel(pos, cmd);
        }
    }

    return MR_OK;
}

void mr_triangle_shader_normalize_barycoord(
    v3f32 barycoord, const struct mr_triangle_shader_inputs* inputs)
{
    v3f32 w_vec = {
        inputs->cmd->verts[0][3],  //
        inputs->cmd->verts[1][3],  //
        inputs->cmd->verts[2][3],  //
    };
    v3f32_mul(barycoord, inputs->barycoord, w_vec);

    const f32 sum = barycoord[0] + barycoord[1] + barycoord[2];
    v3f32_muls(barycoord, barycoord, 1.0f / sum);
}