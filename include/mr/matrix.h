#ifndef MR_MATRIX_H
#define MR_MATRIX_H

#include "types.h"

void mat4f32_identity(mat4f32 mat);
void mat4f32_translate(mat4f32 mat, const v3f32 vec);
void mat4f32_rotate_around_x(mat4f32 mat, f32 rad);
void mat4f32_rotate_around_y(mat4f32 mat, f32 rad);
void mat4f32_rotate_around_z(mat4f32 mat, f32 rad);
void mat4f32_rotate_quat(mat4f32 mat, const v4f32 quat);
void mat4f32_scale(mat4f32 mat, const v3f32 scale);
void mat4f32_scale_scalar(mat4f32 mat, f32 scale);
void mat4f32_perspective(mat4f32 mat, f32 fov, f32 aspect, f32 near, f32 far);

#endif