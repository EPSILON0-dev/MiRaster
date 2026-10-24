#include <string.h>

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

MR_FORCE_STATIC_INLINE void mr_draw_line_resolved(
    struct mr_line_draw_cmd* cmd, v2u16 start, v2u16 end)
{
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
}

MR_FORCE_STATIC_INLINE mr_result_t mr_clamp_line_to_front(v4f32 p0, v4f32 p1)
{
    if (p0[2] >= 0 && p1[2] >= 0)
    {
        return MR_OK;
    }

    if (p0[2] < 0 && p1[2] < 0)
    {
        return MR_OUT_OF_RANGE;
    }

    v2f32 a, b;
    v2f32_sub(a, p1, p0);
    v2f32_divs(a, a, p1[2] - p0[2]);
    v2f32_sub(b, a, p0);
    v2f32_divs(b, b, p0[2]);

    if (p0[2] < 0)
    {
        v2f32_cpy(p0, b);
        p0[2] = 0;
    }
    else
    {
        v2f32_cpy(p1, b);
        p1[2] = 0;
    }

    return MR_MODIFIED;
}

MR_FORCE_STATIC_INLINE mr_result_t mr_clamp_line_to_screen(v2f32 p0, v2f32 p1)
{
    float t_min = 0.0f;
    float t_max = 1.0f;
    v2f32 d;
    v2f32_sub(d, p1, p0);

    v4f32 p = {-d[0], d[0], -d[1], d[1]};
    v4f32 q = {p0[0] + 1.0f, 1.0f - p0[0], p0[1] + 1.0f, 1.0f - p0[1]};

    for (u8 i = 0; i < 4; ++i)
    {
        if (p[i] == 0.0f)
        {
            if (q[i] < 0.0f) return MR_OUT_OF_RANGE;
        }
        else
        {
            f32 t = q[i] / p[i];

            if (p[i] < 0.0f)
                t_min = f32_max(t_min, t);
            else
                t_max = f32_min(t_max, t);

            if (t_min > t_max) return MR_OUT_OF_RANGE;
        }
    }

    p1[0] = p0[0] + d[0] * t_max;
    p1[1] = p0[1] + d[1] * t_max;
    p0[0] += d[0] * t_min;
    p0[1] += d[1] * t_min;
    return MR_MODIFIED;
}

mr_result_t mr_draw_line(struct mr_line_draw_cmd* cmd)
{
    v4f32 verts[2];
    memcpy(verts, cmd->verts, sizeof(verts));

    if (mr_clamp_line_to_front(verts[0], verts[1]) == MR_OUT_OF_RANGE)
    {
        return MR_OUT_OF_RANGE;
    }

    if (mr_clamp_line_to_screen(verts[0], verts[1]) == MR_OUT_OF_RANGE)
    {
        return MR_OUT_OF_RANGE;
    }

    v2u16 iverts[2];
    mr_fragcoord_to_fbcoord(cmd->fb, iverts[0], verts[0]);
    mr_fragcoord_to_fbcoord(cmd->fb, iverts[1], verts[1]);
    mr_draw_line_resolved(cmd, iverts[0], iverts[1]);
    return MR_OK;
}

mr_result_t mr_draw_line_screenbound(struct mr_line_draw_cmd* cmd)
{
    v4f32 verts[2];
    v4f32_cpy(verts[0], cmd->verts[0]);
    v4f32_cpy(verts[1], cmd->verts[1]);
    v4f32_clamp(verts[0], -1.0f, 1.0f);
    v4f32_clamp(verts[1], -1.0f, 1.0f);

    v2u16 iverts[2];
    mr_fragcoord_to_fbcoord(cmd->fb, iverts[0], verts[0]);
    mr_fragcoord_to_fbcoord(cmd->fb, iverts[1], verts[1]);
    mr_draw_line_resolved(cmd, iverts[0], iverts[1]);
    return MR_OK;
}
