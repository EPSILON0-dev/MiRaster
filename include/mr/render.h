#ifndef MR_RENDER_H
#define MR_RENDER_H

#include "mr/fb.h"
#include "mr/types.h"

// ----------------------------------------------------------------------
// Prototypes
// ----------------------------------------------------------------------

struct mr_line_draw_cmd;
struct mr_triangle_draw_cmd;
struct mr_triangle_shader_inputs;
typedef void mr_shader_data;

// ----------------------------------------------------------------------
// Shader typedefs
// ----------------------------------------------------------------------

// Rationale for shader argument format:
//  Since it's a very hot path we want to put as much computation as possible outside of it. If all
//  we supply is a screen position, the shader is welcome to compute whatever it needs out of it.
//  But if all it does is return a color, no time is wasted computing the fragment coordinates.

typedef void (*mr_line_shader_func)(v4f32 color, v2u16 screenpos, struct mr_line_draw_cmd* cmd);
typedef void (*mr_triangle_shader_func)(v4f32 color, struct mr_triangle_shader_inputs* inputs);

// ----------------------------------------------------------------------
// Coord space conversions
// ----------------------------------------------------------------------

void mr_fbcoord_to_fragcoord(struct fb_desc* fb, v2f32 fragcoord, v2u16 fbcoord);
void mr_fragcoord_to_fbcoord(struct fb_desc* fb, v2u16 fbcoord, v2f32 fragcoord);
void mr_fbcoord_to_line_pos(v4f32 pos, f32 *factor, struct fb_desc* fb, v2u16 screenpos, v4f32* verts);

// ----------------------------------------------------------------------
// Screen clearing
// ----------------------------------------------------------------------

// Clear the buffer with the given color
mr_result_t mr_clear_all(struct fb_desc* fb, const v4u8 color);
mr_result_t mr_clear_color(struct fb_desc* fb, const v4u8 color);
mr_result_t mr_clear_depth(struct fb_desc* fb);

// ----------------------------------------------------------------------
// Line drawing
// ----------------------------------------------------------------------

struct mr_line_draw_cmd
{
    struct fb_desc* fb;
    v4f32* verts;
    mr_line_shader_func shader;
    mr_shader_data* data;
};

typedef void __mr_internal_line_pixel_data;
typedef void (*__mr_internal_line_pixel_func)(v2u16 pos, __mr_internal_line_pixel_data* data);

// Internal functions
void __mr_internal_draw_line_low(
    v4u16 verts, __mr_internal_line_pixel_func func, __mr_internal_line_pixel_data* data);
void __mr_internal_draw_line_high(
    v4u16 verts, __mr_internal_line_pixel_func func, __mr_internal_line_pixel_data* data);
void __mr_internal_draw_line_resolved(
    v4u16 verts, __mr_internal_line_pixel_func func, __mr_internal_line_pixel_data* data);
mr_result_t __mr_internal_draw_line(struct mr_line_draw_cmd* cmd,
    __mr_internal_line_pixel_func func, __mr_internal_line_pixel_data* data);

// Render a line, do the clamping and depth testing
mr_result_t mr_draw_line(struct mr_line_draw_cmd* cmd);

// ----------------------------------------------------------------------
// Triangle
// ----------------------------------------------------------------------

struct mr_triangle_shader_inputs
{
    struct fb_desc* fb;
    struct mr_triangle_draw_cmd* cmd;
    mr_shader_data* data;
    v4f32* verts;
    v4f32 pos;
    v2f32 barycoord;
};

struct mr_triangle_draw_cmd
{
    struct fb_desc* fb;
    v4f32* verts;
    mr_triangle_shader_func shader;
    mr_shader_data* data;
};

// We do depth testing and all the fun for all triangles
mr_result_t mr_draw_triangle(struct mr_triangle_draw_cmd* cmd);

#endif
