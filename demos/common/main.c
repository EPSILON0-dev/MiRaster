#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "mr/platform.h"

void render_setup(struct mr_fb_desc* fb);
void render_loop(struct mr_fb_desc* fb, u32 frame);

int main(void)
{
    if (platform_init() != MR_OK)
    {
        platform_fini();
        return 1;
    }

    struct mr_fb_desc fb;
    platform_get_fb_format(&fb);
    size_t fbsz = fb.width * fb.height;
    fb.pixels = malloc(fbsz * sizeof(v4u8));
    fb.depth = malloc(fbsz * sizeof(f32));

    render_setup(&fb);

    u32 frame = 0;
    for (;;)
    {
        bool should_exit = false;
        platform_should_exit(&should_exit);
        if (should_exit) break;

        render_loop(&fb, frame++);

        platform_blit_fb(&fb);

        u32 frametime, rendertime;
        platform_get_last_frame_time_us(&frametime);
        platform_get_render_time_us(&rendertime);
        platform_await_vsync(30);

        if (frame % 30 == 0)
        {
            printf("Render time: %6.2fms, Frame time: %6.2fms       \r", rendertime / 1000.0f,
                frametime / 1000.0f);
        }
        fflush(stdout);
    }

    free(fb.depth);
    free(fb.pixels);
    platform_fini();
    return 0;
}