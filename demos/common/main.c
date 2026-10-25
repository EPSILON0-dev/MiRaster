#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "mr/platform.h"

void render_image(struct fb_desc* fb, u32 frame);

int main(void)
{
    if (platform_init() != MR_OK)
    {
        platform_fini();
        return 1;
    }

    struct fb_desc fb;
    platform_get_fb_format(&fb);
    size_t fbsz = fb.width * fb.height;
    fb.pixels = malloc(fbsz * sizeof(v4u8));
    fb.depth = malloc(fbsz * sizeof(f32));

    u32 frame = 0;
    for (;;)
    {
        bool should_exit = false;
        platform_should_exit(&should_exit);
        if (should_exit) break;

        render_image(&fb, frame++);

        platform_blit_fb(&fb);
        // platform_debug_blit_depth(&fb);
        platform_await_vsync(30);

        u32 frametime;
        platform_get_last_frame_time_us(&frametime);
        printf("Frame time: %6.2fms       \r", frametime / 1000.0f);
        fflush(stdout);

        usleep(10000);
    }

    free(fb.depth);
    free(fb.pixels);
    platform_fini();
    return 0;
}