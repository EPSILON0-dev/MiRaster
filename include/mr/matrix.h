#ifndef MR_MATRIX_H
#define MR_MATRIX_H

#include "types.h"

void mat4_identity(mat4 mat);
void mat4_translate(mat4 mat, const v3 vec);
void mat4_rotate_around_x(mat4 mat, real rad);
void mat4_rotate_around_y(mat4 mat, real rad);
void mat4_rotate_around_z(mat4 mat, real rad);
void mat4_rotate_quat(mat4 mat, const v4 quat);
void mat4_scale(mat4 mat, const v3 scale);
void mat4_scale_scalar(mat4 mat, real scale);
void mat4_perspective(mat4 mat, real fov, real aspect, real near, real far);

#endif
