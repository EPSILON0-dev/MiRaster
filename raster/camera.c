#include "mr/camera.h"

#include "mr/math.h"
#include "mr/matrix.h"

void mr_camera_init(struct mr_camera* cam, f32 yfov, f32 aspect)
{
    v3f32_zeros(cam->pos);
    v3f32_zeros(cam->euler);

    cam->aspect = aspect;
    cam->yfovdegrees = yfov;

    // Safe defaults
    cam->near = 0.01f;
    cam->far = 1000.0f;
}

void mr_camera_vp_matrix(mat4f32 vp, const struct mr_camera* cam)
{
    mat4f32 view, projection;
    mat4f32 rotation_x, rotation_y, rotation_z, translation;
    mat4f32 temp0, temp1;

    mat4f32_rotate_around_x(rotation_x, -cam->euler[0]);
    mat4f32_rotate_around_y(rotation_y, -cam->euler[1]);
    mat4f32_rotate_around_z(rotation_z, -cam->euler[2]);

    v3f32 neg_pos = {
        -cam->pos[0],  //
        -cam->pos[1],  //
        -cam->pos[2]   //
    };

    mat4f32_translate(translation, neg_pos);

    mat4f32_mulmat(temp0, rotation_x, rotation_y);
    mat4f32_mulmat(temp1, temp0, rotation_z);
    mat4f32_mulmat(view, temp1, translation);

    mat4f32_perspective(projection,  //
        cam->yfovdegrees,                   //
        cam->aspect,                 //
        cam->near,                   //
        cam->far                     //
    );

    mat4f32_mulmat(vp, projection, view);
}