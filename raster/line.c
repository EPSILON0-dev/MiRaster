#include "mr/math.h"
#include "mr/render.h"

// Bresenham's Line Algorithm
// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

MR_FORCE_STATIC_INLINE void draw_shaded_pixel(struct mr_line_draw_cmd* cmd, v2u16 pos)
{
    v4f32 fcolor;
    v4u8 ucolor;
    cmd->shader(fcolor, pos, cmd);
    mr_color_f32_to_u8(ucolor, fcolor);
    mr_fb_draw_pixel(cmd->fb, pos, ucolor);
}

MR_FORCE_STATIC_INLINE void draw_line_low(
    struct mr_line_draw_cmd* cmd, u16 x0, u16 y0, u16 x1, u16 y1)
{
    i32 dx = x1 - x0;
    i32 dy = y1 - y0;
    i32 yi = 1;

    if (dy < 0)
    {
        yi = -1;
        dy = -dy;
    }
    i32 d = 2 * dy - dx;
    i32 y = y0;

    for (i32 x = x0; x <= x1; x++)
    {
        v2u16 pos = {x, y};
        draw_shaded_pixel(cmd, pos);

        if (d > 0)
        {
            y += yi;
            d += (2 * (dy - dx));
        }
        else
        {
            d += 2 * dy;
        }
    }
}

MR_FORCE_STATIC_INLINE void draw_line_high(
    struct mr_line_draw_cmd* cmd, u16 x0, u16 y0, u16 x1, u16 y1)
{
    i32 dx = x1 - x0;
    i32 dy = y1 - y0;
    i32 xi = 1;

    if (dx < 0)
    {
        xi = -1;
        dx = -dx;
    }
    i32 d = 2 * dx - dy;
    i32 x = x0;

    for (i32 y = y0; y <= y1; y++)
    {
        v2u16 pos = {x, y};
        draw_shaded_pixel(cmd, pos);

        if (d > 0)
        {
            x += xi;
            d += (2 * (dx - dy));
        }
        else
        {
            d += 2 * dx;
        }
    }
}

mr_result_t mr_draw_line(struct mr_line_draw_cmd* cmd)
{
    // Copy and pack the verts.
    // Clamp them using one SIMD operation.
    // Unpack and convert to screen space pixel coords.
    v4f32 verts;
    v2u16 start, end;
    verts[0] = cmd->verts[0][0];
    verts[1] = cmd->verts[0][1];
    verts[2] = cmd->verts[1][0];
    verts[3] = cmd->verts[1][1];
    v4f32_clamp(verts, -1.0f, 1.0f);
    mr_fragcoord_to_fbcoord(cmd->fb, start, &verts[0]);
    mr_fragcoord_to_fbcoord(cmd->fb, end, &verts[2]);

    i32 dx = i32_abs((i32)end[0] - (i32)start[0]);
    i32 dy = i32_abs((i32)end[1] - (i32)start[1]);
    if (dy < dx)
    {
        if (start[0] > end[0])
            draw_line_low(cmd, end[0], end[1], start[0], start[1]);
        else
            draw_line_low(cmd, start[0], start[1], end[0], end[1]);
    }
    else
    {
        if (start[1] > end[1])
            draw_line_high(cmd, end[0], end[1], start[0], start[1]);
        else
            draw_line_high(cmd, start[0], start[1], end[0], end[1]);
    }

    return MR_OK;
}
