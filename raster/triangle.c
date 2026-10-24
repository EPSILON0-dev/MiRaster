#include <stdlib.h>

#include "mr/fb.h"
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

MR_FORCE_STATIC_INLINE mr_result_t allocate_edge_buff(struct fb_desc* fb)
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

mr_result_t mr_draw_triangle(struct mr_triangle_draw_cmd* cmd)
{
    // Buffer allocation guard
    if (s_triange_renderer_store.edge_buff == NULL ||
        s_triange_renderer_store.fb_height != cmd->fb->height)
    {
        mr_result_t res = allocate_edge_buff(cmd->fb);
        if (res != MR_OK) return res;
    }

    return MR_OK;
}