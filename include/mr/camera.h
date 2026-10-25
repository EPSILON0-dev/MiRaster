#ifndef MR_CAMERA_H
#define MR_CAMERA_H

#include "mr/types.h"

struct mr_camera
{
    v3f32 pos;
    v3f32 euler;
    f32 yfovdegrees;
    f32 aspect;
    f32 near;
    f32 far;
};

void mr_camera_init(struct mr_camera* cam, f32 yfovdegrees, f32 aspect);
void mr_camera_vp_matrix(mat4f32 vp, const struct mr_camera* cam);

#endif