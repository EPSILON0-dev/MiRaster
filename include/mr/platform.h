#ifndef MR_PLATFORM_H
#define MR_PLATFORM_H

#include "mr/types.h"
#include "mr/fb.h"

mr_result_t platform_init(void);
mr_result_t platform_fini(void);

mr_result_t platform_get_fb_format(struct fb_desc *desc);
mr_result_t platform_blit_fb(struct fb_desc *desc);
mr_result_t platform_await_vsync(u16 framerate);
mr_result_t platform_get_last_frame_time_us(u32 *us);
mr_result_t platform_should_exit(bool *should_exit);

#endif
