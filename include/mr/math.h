#ifndef MR_MATH_H
#define MR_MATH_H

#include "mr/config.h"
#include "mr/types.h"

// ----------------------------------------------------------------------
// Vector type implementations
// ----------------------------------------------------------------------

#ifdef MR_MATH_IMPL_AUTOVECTORIZATION
#include "mr/impl/vec_math_impl.h"  // IWYU pragma: keep
__vec_typedef(__vec_float_type_define_impl, f32);
__vec_typedef(__vec_float_type_define_impl, f64);
__vec_typedef(__vec_int_type_define_impl, i8);
__vec_typedef(__vec_int_type_define_impl, i16);
__vec_typedef(__vec_int_type_define_impl, i32);
__vec_typedef(__vec_int_type_define_impl, i64);
__vec_typedef(__vec_uint_type_define_impl, u8);
__vec_typedef(__vec_uint_type_define_impl, u16);
__vec_typedef(__vec_uint_type_define_impl, u32);
__vec_typedef(__vec_uint_type_define_impl, u64);

__vec_f32_dist_impl(2);
__vec_f32_dist_impl(3);
__vec_f32_dist_impl(4);
#endif

#ifdef MR_MATH_IMPL_AUTOVECTORIZATION
#include "mr/impl/mat_math_impl.h"  // IWYU pragma: keep
__mat_typedef(__mat_type_define_impl, f32);
__mat_typedef(__mat_type_define_impl, f64);
#endif

// ----------------------------------------------------------------------
// Scalar type implementations
// ----------------------------------------------------------------------

#include "mr/impl/scal_math_impl.h"  // IWYU pragma: keep

#define __scal_int_typedef(X) \
    X(i8);                    \
    X(i16);                   \
    X(i32);                   \
    X(i64);

#define __scal_uint_typedef(X) \
    X(u8);                     \
    X(u16);                    \
    X(u32);                    \
    X(u64);

#define __scal_float_typedef(X) \
    X(f32);                     \
    X(f64);

#define __scal_typedef(X)   \
    __scal_int_typedef(X);  \
    __scal_uint_typedef(X); \
    __scal_float_typedef(X);

// abs, sign
__scal_int_typedef(__scal_abs);
__scal_float_typedef(__scal_abs);
__scal_int_typedef(__scal_sign);
__scal_float_typedef(__scal_sign);

// clamp, min, max
__scal_typedef(__scal_clamp);
__scal_typedef(__scal_min);
__scal_typedef(__scal_max);

// ----------------------------------------------------------------------
// Type specific implementations
// ----------------------------------------------------------------------

MR_FORCE_STATIC_INLINE void mr_color_f32_to_u8(v4u8 out, v4f32 in)
{
    for (u8 i = 0; i < 4; i++)
    {
        out[i] = (u8)(f32_clamp(in[i], 0.0f, 1.0f) * 255.0f);
    }
}

MR_FORCE_STATIC_INLINE void mr_color_u8_to_f32(v4f32 out, v4u8 in)
{
    for (u8 i = 0; i < 4; i++)
    {
        out[i] = (f32)(in[i]) / 255.0f;
    }
}

#define MR_RAD2DEG(x) ((x) * (180.0f / (f32)M_PI))
#define MR_DEG2RAD(x) ((x) * ((f32)M_PI / 180.0f))

#endif
