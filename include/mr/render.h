#ifndef MR_RENDER_H
#define MR_RENDER_H

#include "mr/fb.h"
#include "mr/types.h"

// ----------------------------------------------------------------------
// Prototypes
// ----------------------------------------------------------------------

struct mr_line_draw_cmd;
typedef void mr_shader_data;

// ----------------------------------------------------------------------
// Shader typedefs
// ----------------------------------------------------------------------

// Rationale for shader argument format:
//  Since it's a very hot path we want to put as much computation as possible outside of it. If all
//  we supply is a screen position, the shader is welcome to compute whatever it needs out of it.
//  But if all it does is return a color, no time is wasted computing the fragment coordinates.

typedef void (*mr_line_shader_func)(v4f32 color, v2u16 screenpos, struct mr_line_draw_cmd* cmd);

// ----------------------------------------------------------------------
// Coord space conversions
// ----------------------------------------------------------------------

void mr_fbcoord_to_fragcoord(struct fb_desc* fb, v2f32 fragcoord, v2u16 fbcoord);
void mr_fragcoord_to_fbcoord(struct fb_desc* fb, v2u16 fbcoord, v2f32 fragcoord);

// ----------------------------------------------------------------------
// Screen clearing
// ----------------------------------------------------------------------

// Clear the buffer with the given color
mr_result_t mr_clear(struct fb_desc* fb, const v4u8 color);

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

// Render a line on without clamping or depth testing
mr_result_t mr_draw_line_screenbound(struct mr_line_draw_cmd* cmd);

// Render a line, do the clamping and depth testing
mr_result_t mr_draw_line(struct mr_line_draw_cmd* cmd);

#endif
