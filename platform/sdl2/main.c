#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "mr/math.h"
#include "mr/platform.h"
#include "mr/render.h"
#include "platform/sdl2/config.h"

// clang-format off
v4f32 cubebase[] =
{
    { -1.0f, -1.0f, -1.0f, 1.0f }, {  1.0f, -1.0f, -1.0f, 1.0f },
    { -1.0f,  1.0f, -1.0f, 1.0f }, {  1.0f,  1.0f, -1.0f, 1.0f },
    { -1.0f, -1.0f,  1.0f, 1.0f }, {  1.0f, -1.0f,  1.0f, 1.0f },
    { -1.0f,  1.0f,  1.0f, 1.0f }, {  1.0f,  1.0f,  1.0f, 1.0f },

    { -1.0f, -1.0f, -1.0f, 1.0f }, { -1.0f,  1.0f, -1.0f, 1.0f },
    {  1.0f, -1.0f, -1.0f, 1.0f }, {  1.0f,  1.0f, -1.0f, 1.0f },
    { -1.0f, -1.0f,  1.0f, 1.0f }, { -1.0f,  1.0f,  1.0f, 1.0f },
    {  1.0f, -1.0f,  1.0f, 1.0f }, {  1.0f,  1.0f,  1.0f, 1.0f },

    { -1.0f, -1.0f, -1.0f, 1.0f }, { -1.0f, -1.0f,  1.0f, 1.0f },
    {  1.0f, -1.0f, -1.0f, 1.0f }, {  1.0f, -1.0f,  1.0f, 1.0f },
    { -1.0f,  1.0f, -1.0f, 1.0f }, { -1.0f,  1.0f,  1.0f, 1.0f },
    {  1.0f,  1.0f, -1.0f, 1.0f }, {  1.0f,  1.0f,  1.0f, 1.0f },
};
// clang-format on

void white_line_shader(v4f32 color, v2u16 screenpos, struct mr_line_draw_cmd* cmd)
{
    (void)cmd;
    (void)screenpos;
    for (u8 i = 0; i < 4; i++) color[i] = 1.0f;
}

static void render_image(struct fb_desc* fb, u32 frame)
{
    const v4u8 clear_color = {0, 0, 0, 255};
    mr_clear(fb, clear_color);

    // Note: Since we store matrices column by column their deifinitions have to be transposed

    mat4f32 scale_mat = {
        0.9f, 0.0f, 0.0f, 0.0f,  //
        0.0f, 0.9f, 0.0f, 0.0f,  //
        0.0f, 0.0f, 0.9f, 0.0f,  //
        0.0f, 0.0f, 0.0f, 1.0f   //
    };

    // clang-format off
    const f32 phi = ((f32)frame) / 100.0f;
    const mat4f32 rotate_mat = {
         cosf(phi), 0.0f, sinf(phi), 0.0f,
              0.0f, 1.0f,      0.0f, 0.0f,
        -sinf(phi), 0.0f, cosf(phi), 0.0f,
              0.0f, 0.0f,      0.0f, 1.0f
    };
    // clang-format on

    const mat4f32 translate_mat = {
        1.0f, 0.0f, 0.0f, 0.0f,  //
        0.0f, 1.0f, 0.0f, 0.0f,  //
        0.0f, 0.0f, 1.0f, 0.0f,  //
        0.0f, 0.0f, -0.3f, 1.0f  //
    };

    // clang-format off
    const f32 aspect = (f32)MR_SDL2_DEFAULT_WIDTH / (f32)MR_SDL2_DEFAULT_HEIGHT;
    const mat4f32 perspective_mat = {
        1.0f / aspect, 0.0f, 0.0f, 0.0f,
        0.0f,          1.0f, 0.0f, 0.0f,
        0.0f,          0.0f, 1.0f, 1.0f,
        0.0f,          0.0f, 0.0f, 0.0f
    };
    // clang-format on

    mat4f32 transform_mat;
    mat4f32_mulmat(transform_mat, scale_mat, rotate_mat);
    mat4f32_mulmat(transform_mat, translate_mat, transform_mat);
    mat4f32_mulmat(transform_mat, perspective_mat, transform_mat);

    v4f32 line[2];
    struct mr_line_draw_cmd cmd = {
        .fb = fb,
        .verts = line,
        .shader = white_line_shader,
        .data = NULL,
    };

    for (size_t i = 0; i < sizeof(cubebase) / sizeof(*cubebase); i += 2)
    {
        for (size_t j = 0; j < 2; j++)
        {
            mat4f32_mulvec(line[j], transform_mat, cubebase[i + j]);
            f32 w = line[j][3];
            line[j][3] = 1.0f;
            v4f32_divs(line[j], line[j], f32_abs(w));
        }

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
