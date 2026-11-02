#include "mr/matrix.h"

#include <string.h>

#include "mr/math.h"  // IWYU pragma: keep
#include "mr/types.h"

void mat4_identity(mat4 mat)
{
    memset(mat, 0, sizeof(mat4));
    mat[0] = REAL(1);
    mat[5] = REAL(1);
    mat[10] = REAL(1);
    mat[15] = REAL(1);
}

void mat4_translate(mat4 mat, const v3 vec)
{
    mat4_identity(mat);
    mat[12] = vec[0];
    mat[13] = vec[1];
    mat[14] = vec[2];
}

void mat4_rotate_around_x(mat4 mat, real phi)
{
    const real c = real_cos(phi);
    const real s = real_sin(phi);

    memset(mat, 0, sizeof(mat4));

    mat[0] = REAL(1);
    mat[5] = c;
    mat[6] = s;
    mat[9] = -s;
    mat[10] = c;
    mat[15] = REAL(1);
}

void mat4_rotate_around_y(mat4 mat, real phi)
{
    const real c = real_cos(phi);
    const real s = real_sin(phi);

    memset(mat, 0, sizeof(mat4));

    mat[0] = c;
    mat[2] = -s;
    mat[5] = REAL(1);
    mat[8] = s;
    mat[10] = c;
    mat[15] = REAL(1);
}

void mat4_rotate_around_z(mat4 mat, real phi)
{
    const real c = real_cos(phi);
    const real s = real_sin(phi);

    memset(mat, 0, sizeof(mat4));

    mat[0] = c;
    mat[1] = s;
    mat[4] = -s;
    mat[5] = c;
    mat[10] = REAL(1);
    mat[15] = REAL(1);
}

void mat4_rotate_quat(mat4 mat, const v4 quat)
{
    const real x = quat[0];
    const real y = quat[1];
    const real z = quat[2];
    const real w = quat[3];

    const real xx = real_mul(x, x);
    const real yy = real_mul(y, y);
    const real zz = real_mul(z, z);
    const real xy = real_mul(x, y);
    const real xz = real_mul(x, z);
    const real yz = real_mul(y, z);
    const real wx = real_mul(w, x);
    const real wy = real_mul(w, y);
    const real wz = real_mul(w, z);

    memset(mat, 0, sizeof(mat4));

    mat[0] = real_sub(REAL(1), real_mul(REAL(2), real_add(yy, zz)));
    mat[1] = real_mul(REAL(2), real_add(xy, wz));
    mat[2] = real_mul(REAL(2), real_sub(xz, wy));

    mat[4] = real_mul(REAL(2), real_sub(xy, wz));
    mat[5] = real_sub(REAL(1), real_mul(REAL(2), real_add(xx, zz)));
    mat[6] = real_mul(REAL(2), real_add(yz, wx));

    mat[8] = real_mul(REAL(2), real_add(xz, wy));
    mat[9] = real_mul(REAL(2), real_sub(yz, wx));
    mat[10] = real_sub(REAL(1), real_mul(REAL(2), real_add(xx, yy)));

    mat[15] = REAL(1);
}

void mat4_scale(mat4 mat, const v3 scale)
{
    memset(mat, 0, sizeof(mat4));
    mat[0] = scale[0];
    mat[5] = scale[1];
    mat[10] = scale[2];
    mat[15] = REAL(1);
}

void mat4_scale_scalar(mat4 mat, real scale)
{
    memset(mat, 0, sizeof(mat4));
    mat[0] = scale;
    mat[5] = scale;
    mat[10] = scale;
    mat[15] = REAL(1);
}

void mat4_perspective(mat4 mat, real yfovdegrees, real aspect, real near, real far)
{
    const real f = real_div(REAL(1), real_tan(real_mul(MR_DEG2RAD(yfovdegrees), REAL(0.5f))));

    memset(mat, 0, sizeof(mat4));

    mat[0] = real_div(f, aspect);
    mat[5] = real_inv(f);
    mat[10] = real_div(real_add(far, near), real_sub(far, near));
    mat[11] = REAL(1);
    mat[14] = real_div(real_mul(REAL(2), real_mul(far, near)), real_sub(far, near));
}
