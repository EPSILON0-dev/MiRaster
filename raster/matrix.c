#include "mr/matrix.h"

#include <math.h>
#include <string.h>

#include "mr/math.h"
#include "mr/types.h"

void mat4f32_identity(mat4f32 mat)
{
    memset(mat, 0, sizeof(mat4f32));
    mat[0] = 1.0f;
    mat[5] = 1.0f;
    mat[10] = 1.0f;
    mat[15] = 1.0f;
}

void mat4f32_translate(mat4f32 mat, const v3f32 vec)
{
    mat4f32_identity(mat);
    mat[12] = vec[0];
    mat[13] = vec[1];
    mat[14] = vec[2];
}

void mat4f32_rotate_around_x(mat4f32 mat, f32 phi)
{
    const f32 c = cosf(phi);
    const f32 s = sinf(phi);

    memset(mat, 0, sizeof(mat4f32));

    mat[0] = 1.0f;
    mat[5] = c;
    mat[6] = s;
    mat[9] = -s;
    mat[10] = c;
    mat[15] = 1.0f;
}

void mat4f32_rotate_around_y(mat4f32 mat, f32 phi)
{
    const f32 c = cosf(phi);
    const f32 s = sinf(phi);

    memset(mat, 0, sizeof(mat4f32));

    mat[0] = c;
    mat[2] = -s;
    mat[5] = 1.0f;
    mat[8] = s;
    mat[10] = c;
    mat[15] = 1.0f;
}

void mat4f32_rotate_around_z(mat4f32 mat, f32 phi)
{
    const f32 c = cosf(phi);
    const f32 s = sinf(phi);

    memset(mat, 0, sizeof(mat4f32));

    mat[0] = c;
    mat[1] = s;
    mat[4] = -s;
    mat[5] = c;
    mat[10] = 1.0f;
    mat[15] = 1.0f;
}

void mat4f32_rotate_quat(mat4f32 mat, const v4f32 quat)
{
    const f32 x = quat[0];
    const f32 y = quat[1];
    const f32 z = quat[2];
    const f32 w = quat[3];

    const f32 xx = x * x;
    const f32 yy = y * y;
    const f32 zz = z * z;
    const f32 xy = x * y;
    const f32 xz = x * z;
    const f32 yz = y * z;
    const f32 wx = w * x;
    const f32 wy = w * y;
    const f32 wz = w * z;

    memset(mat, 0, sizeof(mat4f32));

    mat[0] = 1.0f - 2.0f * (yy + zz);
    mat[1] = 2.0f * (xy + wz);
    mat[2] = 2.0f * (xz - wy);

    mat[4] = 2.0f * (xy - wz);
    mat[5] = 1.0f - 2.0f * (xx + zz);
    mat[6] = 2.0f * (yz + wx);

    mat[8] = 2.0f * (xz + wy);
    mat[9] = 2.0f * (yz - wx);
    mat[10] = 1.0f - 2.0f * (xx + yy);

    mat[15] = 1.0f;
}

void mat4f32_scale(mat4f32 mat, const v3f32 scale)
{
    memset(mat, 0, sizeof(mat4f32));
    mat[0] = scale[0];
    mat[5] = scale[1];
    mat[10] = scale[2];
    mat[15] = 1.0f;
}

void mat4f32_scale_scalar(mat4f32 mat, f32 scale)
{
    memset(mat, 0, sizeof(mat4f32));
    mat[0] = scale;
    mat[5] = scale;
    mat[10] = scale;
    mat[15] = 1.0f;
}

void mat4f32_perspective(mat4f32 mat, f32 yfovdegrees, f32 aspect, f32 near, f32 far)
{
    const f32 f = 1.0f / tanf(MR_DEG2RAD(yfovdegrees) * 0.5f);

    memset(mat, 0, sizeof(mat4f32));

    mat[0] = f / aspect;
    mat[5] = f;
    mat[10] = (far + near) / (far - near);
    mat[11] = 1.0f;
    mat[14] = (2.0f * far * near) / (far - near);
}
