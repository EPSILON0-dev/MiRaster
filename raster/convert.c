#include "mr/fb.h"
#include "mr/math.h"

void mr_fbcoord_to_fragcoord(struct fb_desc* fb, v2f32 fragcoord, v2u16 fbcoord)
{
    fragcoord[0] = ((f32)fbcoord[0]) / (fb->width - 1) * 2.0f - 1.0f;
    fragcoord[1] = ((f32)fbcoord[1]) / (fb->height - 1) * 2.0f - 1.0f;
}

void mr_fragcoord_to_fbcoord(struct fb_desc* fb, v2u16 fbcoord, v2f32 fragcoord)
{
    fbcoord[0] = (u16)((fragcoord[0] * 0.5f + 0.5f) * (fb->width - 1));
    fbcoord[1] = (u16)((fragcoord[1] * 0.5f + 0.5f) * (fb->height - 1));
}

void mr_fbcoord_to_line_pos(
    v4f32 pos, f32* factor, struct fb_desc* fb, v2u16 screenpos, v4f32* verts)
{
    v2f32 coord;
    mr_fbcoord_to_fragcoord(fb, coord, screenpos);

    const f32 d0 = v2f32_dist(coord, verts[0]);
    const f32 d1 = v2f32_dist(coord, verts[1]);
    const f32 f = d0 / (d0 + d1);

    if (factor) *factor = f;
    v4f32_lerp(pos, verts[0], verts[1], f);
}

void mr_fbcoord_to_triangle_pos(
    v4f32 pos, v3f32 barycoord, struct fb_desc* fb, v2u16 screenpos, v4f32* verts)
{
    v3f32 coord;
    mr_fbcoord_to_fragcoord(fb, coord, screenpos);

    const f32 d = (verts[1][1] - verts[2][1]) * (verts[0][0] - verts[2][0]) +  //
                  (verts[2][0] - verts[1][0]) * (verts[0][1] - verts[2][1]);   //

    if (d == 0.0f) return;

    const f32 inv_d = 1.0f / d;

    barycoord[0] = ((verts[1][1] - verts[2][1]) * (coord[0] - verts[2][0]) +   //
                       (verts[2][0] - verts[1][0]) * (coord[1] - verts[2][1])  //
                       ) *
                   inv_d;

    barycoord[1] = ((verts[2][1] - verts[0][1]) * (coord[0] - verts[2][0]) +   //
                       (verts[0][0] - verts[2][0]) * (coord[1] - verts[2][1])  //
                       ) *
                   inv_d;

    barycoord[2] = 1.0f - barycoord[0] - barycoord[1];

    v4f32_muls(pos, verts[0], barycoord[0]);
    v4f32_madds(pos, verts[1], barycoord[1], pos);
    v4f32_madds(pos, verts[2], barycoord[2], pos);

    v4f32_muls(pos, verts[0], barycoord[0]);
    v4f32_madds(pos, verts[1], barycoord[1], pos);
    v4f32_madds(pos, verts[2], barycoord[2], pos);
}