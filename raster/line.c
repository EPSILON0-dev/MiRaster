#include <string.h>

#include "mr/fb.h"
#include "mr/math.h"
#include "mr/render.h"
#include "mr/types.h"

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

void __mr_internal_draw_line_low(
    v4u16 verts, __mr_internal_line_pixel_func func, __mr_internal_line_pixel_data* data)
{
    const u16 x0 = verts[0];
    const u16 y0 = verts[1];
    const u16 x1 = verts[2];
    const u16 y1 = verts[3];

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
        func(pos, data);

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

void __mr_internal_draw_line_high(
    v4u16 verts, __mr_internal_line_pixel_func func, __mr_internal_line_pixel_data* data)
{
    const u16 x0 = verts[0];
    const u16 y0 = verts[1];
    const u16 x1 = verts[2];
    const u16 y1 = verts[3];

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
        func(pos, data);

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

MR_FORCE_STATIC_INLINE void swap_screencoord_verts(v4u16 verts)
{
    const u16 tx = verts[0];
    verts[0] = verts[2];
    verts[2] = tx;

    const u16 ty = verts[1];
    verts[1] = verts[3];
    verts[3] = ty;
}

void __mr_internal_draw_line_resolved(
    v4u16 verts, __mr_internal_line_pixel_func func, __mr_internal_line_pixel_data* data)
{
    i32 dx = i32_abs((i32)verts[2 + 0] - (i32)verts[0]);
    i32 dy = i32_abs((i32)verts[2 + 1] - (i32)verts[1]);
    if (dy < dx)
    {
        if (verts[0] > verts[2 + 0]) swap_screencoord_verts(verts);
        __mr_internal_draw_line_low(verts, func, data);
    }
    else
    {
        if (verts[1] > verts[2 + 1]) swap_screencoord_verts(verts);
        __mr_internal_draw_line_high(verts, func, data);
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

    const f32 t = -p0[2] / (p1[2] - p0[2]);

    v2f32 d, intersection;
    v2f32_sub(d, p1, p0);
    v2f32_muls(d, d, t);
    v2f32_add(intersection, p0, d);

    if (p0[2] < 0)
    {
        v2f32_cpy(p0, intersection);
        p0[2] = 0;
    }
    else
    {
        v2f32_cpy(p1, intersection);
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

mr_result_t __mr_internal_draw_line(struct mr_line_draw_cmd* cmd,
    __mr_internal_line_pixel_func func, __mr_internal_line_pixel_data* data)
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

    v4u16 iverts;
    mr_fragcoord_to_fbcoord(cmd->fb, &iverts[0], verts[0]);
    mr_fragcoord_to_fbcoord(cmd->fb, &iverts[2], verts[1]);
    __mr_internal_draw_line_resolved(iverts, func, data);
    return MR_OK;
}

static void line_draw_pixel_func(v2u16 pos, __mr_internal_line_pixel_data* data)
{
    struct mr_line_draw_cmd* cmd = data;
    draw_shaded_pixel(cmd, pos);
}

mr_result_t mr_draw_line(struct mr_line_draw_cmd* cmd)
{
    return __mr_internal_draw_line(cmd, line_draw_pixel_func, cmd);
}
