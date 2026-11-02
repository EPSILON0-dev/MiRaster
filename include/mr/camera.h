#ifndef MR_CAMERA_H
#define MR_CAMERA_H

#include "mr/types.h"

struct mr_camera
{
    v3 pos;
    v3 euler;
    real yfovdegrees;
    real aspect;
    real near;
    real far;
};

void mr_camera_init(struct mr_camera* cam, real yfovdegrees, real aspect);
void mr_camera_vp_matrix(mat4 vp, const struct mr_camera* cam);

#endif
