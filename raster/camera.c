#include "mr/camera.h"

#include "mr/math.h"  // IWYU pragma: keep
#include "mr/matrix.h"

void mr_camera_init(struct mr_camera* cam, real yfov, real aspect)
{
    v3_zeros(cam->pos);
    v3_zeros(cam->euler);

    cam->aspect = aspect;
    cam->yfovdegrees = yfov;

    // Safe defaults
    cam->near = REAL(0.01f);
    cam->far = REAL(1000.0f);
}

void mr_camera_vp_matrix(mat4 vp, const struct mr_camera* cam)
{
    mat4 view, projection;
    mat4 rotation_x, rotation_y, rotation_z, translation;
    mat4 temp0, temp1;

    mat4_rotate_around_x(rotation_x, real_inv(cam->euler[0]));
    mat4_rotate_around_y(rotation_y, real_inv(cam->euler[1]));
    mat4_rotate_around_z(rotation_z, real_inv(cam->euler[2]));

    v3 neg_pos = {
        real_inv(cam->pos[0]),  //
        real_inv(cam->pos[1]),  //
        real_inv(cam->pos[2])   //
    };

    mat4_translate(translation, neg_pos);

    mat4_mulmat(temp0, rotation_x, rotation_y);
    mat4_mulmat(temp1, temp0, rotation_z);
    mat4_mulmat(view, temp1, translation);

    mat4_perspective(projection,  //
        cam->yfovdegrees,         //
        cam->aspect,              //
        cam->near,                //
        cam->far                  //
    );

    mat4_mulmat(vp, projection, view);
}
