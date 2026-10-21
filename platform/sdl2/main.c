#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mr/math.h"
#include "mr/platform.h"
#include "mr/render.h"
#include "platform/sdl2/config.h"

// clang-format off
v4f32 cubebase[] =
{
    { -1.0f, -1.0f, -1.0f, 0.0f }, {  1.0f, -1.0f, -1.0f, 0.0f },
    { -1.0f,  1.0f, -1.0f, 0.0f }, {  1.0f,  1.0f, -1.0f, 0.0f },
    { -1.0f, -1.0f,  1.0f, 0.0f }, {  1.0f, -1.0f,  1.0f, 0.0f },
    { -1.0f,  1.0f,  1.0f, 0.0f }, {  1.0f,  1.0f,  1.0f, 0.0f },

    { -1.0f, -1.0f, -1.0f, 0.0f }, { -1.0f,  1.0f, -1.0f, 0.0f },
    {  1.0f, -1.0f, -1.0f, 0.0f }, {  1.0f,  1.0f, -1.0f, 0.0f },
    { -1.0f, -1.0f,  1.0f, 0.0f }, { -1.0f,  1.0f,  1.0f, 0.0f },
    {  1.0f, -1.0f,  1.0f, 0.0f }, {  1.0f,  1.0f,  1.0f, 0.0f },

    { -1.0f, -1.0f, -1.0f, 0.0f }, { -1.0f, -1.0f,  1.0f, 0.0f },
    {  1.0f, -1.0f, -1.0f, 0.0f }, {  1.0f, -1.0f,  1.0f, 0.0f },
    { -1.0f,  1.0f, -1.0f, 0.0f }, { -1.0f,  1.0f,  1.0f, 0.0f },
    {  1.0f,  1.0f, -1.0f, 0.0f }, {  1.0f,  1.0f,  1.0f, 0.0f },
};
// clang-format on

v4f32 cubexformbuff[sizeof(cubebase) / sizeof(*cubebase)];

void white_line_shader(v4f32 color, v2u16 screenpos, struct mr_line_draw_cmd* cmd)
{
    (void)cmd;
    (void)screenpos;
    for (u8 i = 0; i < 4; i++) color[i] = 1.0f;
}

static void render_image(struct fb_desc* fb, u32 frame)
{
    (void)frame;

    const v4u8 clear_color = {0, 0, 0, 255};
    mr_clear(fb, clear_color);

    const f32 phi = ((f32)frame) / 100.0f;

    const f32 aspect = (f32)MR_SDL2_DEFAULT_WIDTH / (f32)MR_SDL2_DEFAULT_HEIGHT;
    const v4f32 aspectvec = {1.0f, 1.0f / aspect, 1.0f, 1.0f};

    for (size_t i = 0; i < sizeof(cubebase) / sizeof(*cubebase); i++)
    {
        v4f32 offset = {0.0f, 0.0f, 3.0f, 0.0f};

        // Copy the original cube
        v4f32_cpy(cubexformbuff[i], cubebase[i]);

        // Scale by a scale factor
        v4f32_muls(cubexformbuff[i], cubexformbuff[i], 0.9f);

        // Apply rotation (no matrix yet :c)
        const f32 x = cubexformbuff[i][0];
        const f32 z = cubexformbuff[i][2];
        cubexformbuff[i][0] = x * cosf(phi) + z * sinf(phi);
        cubexformbuff[i][2] = -x * sinf(phi) + z * cosf(phi);

        // Offset the cube from the camera
        v4f32_add(cubexformbuff[i], cubexformbuff[i], offset);

        // Apply the perspective
        v4f32_divs(cubexformbuff[i], cubexformbuff[i], cubexformbuff[i][2]);

        // Scale by aspect ratio
        v4f32_div(cubexformbuff[i], cubexformbuff[i], aspectvec);
    }

    struct mr_line_draw_cmd cmd = {
        .fb = fb,
        .verts = NULL,
        .shader = white_line_shader,
        .data = NULL,
    };

    for (size_t i = 0; i < sizeof(cubexformbuff) / sizeof(*cubexformbuff); i += 2)
    {
        cmd.verts = &cubexformbuff[i];
        mr_draw_line(&cmd);
    }
}

int main(void)
{
    if (platform_init() != MR_OK)
    {
        platform_fini();
        return 1;
    }

    struct fb_desc fb;
    platform_get_fb_format(&fb);
    size_t fbsz = fb.width * fb.height * 4;
    fb.pixels = malloc(fbsz);

    u32 frame = 0;
    for (;;)
    {
        bool should_exit = false;
        platform_should_exit(&should_exit);
        if (should_exit) break;

        render_image(&fb, frame++);

        platform_blit_fb(&fb);
        platform_await_vsync(30);

        u32 frametime;
        platform_get_last_frame_time_us(&frametime);
        printf("Frame time: %6.2fms       \r", frametime / 1000.0f);
        fflush(stdout);
    }

    free(fb.pixels);
    platform_fini();
    return 0;
}
