#include <string.h>

#include "mr/fb.h"
#include "mr/math.h"  // IWYU pragma: keep
#include "mr/render.h"
#include "mr/types.h"

// Bresenham's Line Algorithm
// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

MR_FORCE_STATIC_INLINE void draw_shaded_pixel(struct mr_line_draw_cmd* cmd, v2u16 pos)
{
    v4 fcolor;
    v4u8 ucolor;
    cmd->shader(fcolor, pos, cmd);
    mr_color_real_to_u8(ucolor, fcolor);
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

MR_FORCE_STATIC_INLINE mr_result_t mr_clamp_line_to_front(v4 p0, v4 p1)
{
    if (real_ge(p0[2], REAL(0)) && real_ge(p1[2], REAL(0)))
    {
        return MR_OK;
    }

    if (real_lt(p0[2], REAL(0)) && real_lt(p1[2], REAL(0)))
    {
        return MR_OUT_OF_RANGE;
    }

    const real t = real_div(real_inv(p0[2]), real_sub(p1[2], p0[2]));

    v2 d, intersection;
    v2_sub(d, p1, p0);
    v2_muls(d, d, t);
    v2_add(intersection, p0, d);

    if (real_lt(p0[2], REAL(0)))
    {
        v2_cpy(p0, intersection);
        p0[2] = REAL(0);
    }
    else
    {
        v2_cpy(p1, intersection);
        p1[2] = REAL(0);
    }

    return MR_MODIFIED;
}

MR_FORCE_STATIC_INLINE mr_result_t mr_clamp_line_to_screen(v2 p0, v2 p1)
{
    real t_min = REAL(0);
    real t_max = REAL(1);
    v2 d;
    v2_sub(d, p1, p0);

    v4 p = {
        real_inv(d[0]),  //
        d[0],            //
        real_inv(d[1]),  //
        d[1]             //
    };
    v4 q = {
        real_add(p0[0], REAL(1)),  //
        real_sub(REAL(1), p0[0]),  //
        real_add(p0[1], REAL(1)),  //
        real_sub(REAL(1), p0[1])   //
    };

    for (u8 i = 0; i < 4; ++i)
    {
        if (real_eq(p[i], REAL(0)))
        {
            if (real_lt(q[i], REAL(0))) return MR_OUT_OF_RANGE;
        }
        else
        {
            const real t = real_div(q[i], p[i]);

            if (real_lt(p[i], REAL(0)))
                t_min = real_max(t_min, t);
            else
                t_max = real_min(t_max, t);

            if (real_gt(t_min, t_max)) return MR_OUT_OF_RANGE;
        }
    }

    p1[0] = real_add(p0[0], real_mul(d[0], t_max));
    p1[1] = real_add(p0[1], real_mul(d[1], t_max));
    p0[0] = real_add(p0[0], real_mul(d[0], t_min));
    p0[1] = real_add(p0[1], real_mul(d[1], t_min));
    return MR_MODIFIED;
}

mr_result_t __mr_internal_draw_line(struct mr_line_draw_cmd* cmd,
    __mr_internal_line_pixel_func func, __mr_internal_line_pixel_data* data)
{
    v4 verts[2];
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
