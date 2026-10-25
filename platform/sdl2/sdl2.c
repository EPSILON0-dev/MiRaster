#include <SDL2/SDL.h>
#include <time.h>
#include <unistd.h>

#include "mr/fb.h"
#include "mr/math.h"
#include "mr/types.h"
#include "platform/sdl2/config.h"

static SDL_Window* s_window;
static SDL_Surface* s_screen_surface;
static SDL_Surface* s_fb_surface;
static u32* s_pixels;
static u16 s_window_width = MR_SDL2_DEFAULT_WIDTH;
static u16 s_window_height = MR_SDL2_DEFAULT_HEIGHT;

mr_result_t platform_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "[platform:SDL2] SDL could not initialize! (%s)\n", SDL_GetError());
        return MR_ERROR;
    }

    s_window = SDL_CreateWindow("MiRaster",  //
        SDL_WINDOWPOS_UNDEFINED,             //
        SDL_WINDOWPOS_UNDEFINED,             //
        s_window_width,                      //
        s_window_height,                     //
        SDL_WINDOW_SHOWN                     //
    );

    if (s_window == NULL)
    {
        fprintf(stderr, "[platform:SDL2] Window could not be created! (%s)\n", SDL_GetError());
        return MR_ERROR;
    }

    s_screen_surface = SDL_GetWindowSurface(s_window);
    if (s_screen_surface->format->format != SDL_PIXELFORMAT_ARGB8888)
    {
        fprintf(stderr, "[platform:SDL2] Invalid pixel format!\n");
        return MR_ERROR;
    }

    s_fb_surface = SDL_CreateRGBSurfaceWithFormat(0,  //
        s_window_width / MR_SDL2_DEFAULT_SCALE,       //
        s_window_height / MR_SDL2_DEFAULT_SCALE,      //
        32,                                           //
        SDL_PIXELFORMAT_ARGB8888                      //
    );

    if (s_fb_surface == NULL)
    {
        fprintf(stderr, "[platform:SDL2] Surface creation failed! (%s)\n", SDL_GetError());
        return MR_ERROR;
    }

    s_pixels = (uint32_t*)s_fb_surface->pixels;

    return MR_OK;
}

mr_result_t platform_fini(void)
{
    SDL_FreeSurface(s_fb_surface);
    SDL_DestroyWindow(s_window);
    SDL_Quit();
    return MR_OK;
}

mr_result_t platform_get_fb_format(struct fb_desc* desc)
{
    desc->width = s_window_width / MR_SDL2_DEFAULT_SCALE;
    desc->height = s_window_height / MR_SDL2_DEFAULT_SCALE;
    desc->channels = 4;
    desc->flags.pixel_type_uint8 = true;
    desc->flags.pixel_format_rgba = true;
    return MR_OK;
}

static mr_result_t validate_fb_format(struct fb_desc* desc)
{
    bool format_valid = true;
    format_valid &= desc->width == s_window_width / MR_SDL2_DEFAULT_SCALE;
    format_valid &= desc->height == s_window_height / MR_SDL2_DEFAULT_SCALE;
    format_valid &= desc->channels == 4;
    format_valid &= desc->flags.pixel_type_uint8 == true;
    format_valid &= desc->flags.pixel_format_rgba == true;
    return format_valid ? MR_OK : MR_INVALID_FORMAT;
}

static u32 swizzle_abgr2rgba(u32 pix)
{
    // clang-format off
    return
        (pix & 0x000000ff) << 16 |
        (pix & 0x0000ff00)       |
        (pix & 0x00ff0000) >> 16 |
        (pix & 0xff000000);
    // clang-format on
}

mr_result_t platform_blit_fb(struct fb_desc* desc)
{
    if (validate_fb_format(desc) != MR_OK)
    {
        fprintf(stderr, "[platform:SDL2] Window could not be created! (%s)\n", SDL_GetError());
        return MR_INVALID_FORMAT;
    }

    {
        SDL_LockSurface(s_fb_surface);
        const size_t fb_pixels = desc->width * desc->height;
        for (size_t i = 0; i < fb_pixels; i++)
        {
            ((u32*)s_pixels)[i] = swizzle_abgr2rgba(((u32*)desc->pixels)[i]);
        }
        SDL_UnlockSurface(s_fb_surface);
    }

    SDL_BlitScaled(s_fb_surface, NULL, s_screen_surface, NULL);
    SDL_UpdateWindowSurface(s_window);
    return MR_OK;
}

mr_result_t platform_debug_blit_depth(struct fb_desc* desc)
{
    if (validate_fb_format(desc) != MR_OK)
    {
        fprintf(stderr, "[platform:SDL2] Window could not be created! (%s)\n", SDL_GetError());
        return MR_INVALID_FORMAT;
    }

    {
        SDL_LockSurface(s_fb_surface);
        const size_t fb_pixels = desc->width * desc->height;
        for (size_t i = 0; i < fb_pixels; i++)
        {
            u8 b = (u8)f32_clamp((1.0f / ((f32*)desc->depth)[i]) * 255.0f / 20.0f, 0.0f, 255.0f);
            u32 color = (255 << 24) | (b << 16) | (b << 8) | (b);
            ((u32*)s_pixels)[i] = color;
        }
        SDL_UnlockSurface(s_fb_surface);
    }

    SDL_BlitScaled(s_fb_surface, NULL, s_screen_surface, NULL);
    SDL_UpdateWindowSurface(s_window);
    return MR_OK;
}

static uint64_t get_time_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

mr_result_t platform_await_vsync(u16 framerate)
{
    (void)framerate;
    return MR_UNIMP;
}

mr_result_t platform_get_last_frame_time_us(u32* us)
{
    static u64 prev_frame = 0;
    u64 now = get_time_ns();
    *us = (now - prev_frame) / 1000;
    prev_frame = now;

    return MR_OK;
}

mr_result_t platform_should_exit(bool* should_exit)
{
    SDL_Event e;
    *should_exit = false;
    while (SDL_PollEvent(&e) != 0)
        if (e.type == SDL_QUIT) *should_exit = true;

    return MR_OK;
}
