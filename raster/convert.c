#include "mr/fb.h"
#include "mr/math.h"

void mr_fbcoord_to_fragcoord(struct mr_fb_desc* fb, v2 fragcoord, v2u16 fbcoord)
{
    fragcoord[0] = real_div(int2real(fbcoord[0]), int2real(fb->width - 1));
    fragcoord[1] = real_div(int2real(fbcoord[1]), int2real(fb->height - 1));

    v2_muls(fragcoord, fragcoord, REAL(2));
    v2_subs(fragcoord, fragcoord, REAL(1));
}

void mr_fragcoord_to_fbcoord(struct mr_fb_desc* fb, v2u16 fbcoord, v2 fragcoord)
{
    v2 t, res = {REAL(fb->width - 1), REAL(fb->height - 1)};
    v2_maddss(t, fragcoord, REAL(0.5f), REAL(0.5f));
    v2_mul(t, t, res);
    fbcoord[0] = real2int(t[0]);
    fbcoord[1] = real2int(t[1]);
}

void mr_fbcoord_to_line_pos(v4 pos, real* factor, struct mr_fb_desc* fb, v2u16 screenpos, v4* verts)
{
    v2 coord;
    mr_fbcoord_to_fragcoord(fb, coord, screenpos);

    const real d0 = v2_dist(coord, verts[0]);
    const real d1 = v2_dist(coord, verts[1]);
    const real f = real_div(d0, real_add(d0, d1));

    if (factor) *factor = f;
    v4_lerp(pos, verts[0], verts[1], f);
}

void mr_fbcoord_to_triangle_pos(
    v4 pos, v3 barycoord, struct mr_fb_desc* fb, v2u16 screenpos, v4* verts)
{
    v3 coord;
    mr_fbcoord_to_fragcoord(fb, coord, screenpos);

    const real d = real_add(
        real_mul(real_sub(verts[1][1], verts[2][1]), real_sub(verts[0][0], verts[2][0])),   //
        real_mul(real_sub(verts[2][0], verts[1][0]), real_sub(verts[0][1], verts[2][1])));  //

    if (real_is_zero(d)) return;

    const real inv_d = real_div(1.0f, d);

    barycoord[0] = real_mul(inv_d,
        real_add(                                                                           //
            real_mul(real_sub(verts[1][1], verts[2][1]), real_sub(coord[0], verts[2][0])),  //
            real_mul(real_sub(verts[2][0], verts[1][0]), real_sub(coord[1], verts[2][1]))   //
            ));

    barycoord[1] = real_mul(inv_d,
        real_add(                                                                           //
            real_mul(real_sub(verts[2][1], verts[0][1]), real_sub(coord[0], verts[2][0])),  //
            real_mul(real_sub(verts[0][0], verts[2][0]), real_sub(coord[1], verts[2][1]))   //
            ));

    barycoord[2] = real_sub(real_sub(1.0f, barycoord[0]), barycoord[1]);

    v4_muls(pos, verts[0], barycoord[0]);
    v4_madds(pos, verts[1], barycoord[1], pos);
    v4_madds(pos, verts[2], barycoord[2], pos);
}
