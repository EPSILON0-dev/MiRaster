#ifndef MR_VEC_MATH_IMPL_H
#define MR_VEC_MATH_IMPL_H

#include <math.h>  // IWYU pragma: keep

#include "mr/math/real_math_impl.h"
#include "mr/types.h"

// ----------------------------------------------------------------------
// Implementations
// ----------------------------------------------------------------------

#define __vec_vec_op_impl(name, operator, base, len)                                           \
    MR_INLINE void __vec_v_opname(name, base, len)(__vec_typename(base, len) res, \
        const __vec_typename(base, len) a, const __vec_typename(base, len) b)                  \
    {                                                                                          \
        for (u8 i = 0; i < len; i++)                                                           \
        {                                                                                      \
            res[i] = a[i] operator b[i];                                                       \
        }                                                                                      \
    }

#define __vec_scal_op_impl(name, operator, base, len)                                   \
    MR_INLINE void __vec_s_opname(name, base, len)(                        \
        __vec_typename(base, len) res, const __vec_typename(base, len) a, const base b) \
    {                                                                                   \
        for (u8 i = 0; i < len; i++)                                                    \
        {                                                                               \
            res[i] = a[i] operator b;                                                   \
        }                                                                               \
    }

#define __vec_clamp(base, len)                                             \
    MR_INLINE void v##len##base##_clamp(                      \
        __vec_typename(base, len) val, const base min, const base max)     \
    {                                                                      \
        for (u8 i = 0; i < len; i++)                                       \
        {                                                                  \
            val[i] = (val[i] < min) ? min : (val[i] > max) ? max : val[i]; \
        }                                                                  \
    }

#define __vec_cpy(base, len)                                                \
    MR_INLINE void v##len##base##_cpy(                         \
        __vec_typename(base, len) dst, const __vec_typename(base, len) src) \
    {                                                                       \
        for (u8 i = 0; i < len; i++)                                        \
        {                                                                   \
            dst[i] = src[i];                                                \
        }                                                                   \
    }

#define __vec_madd(base, len)                                                      \
    MR_INLINE void v##len##base##_madd(__vec_typename(base, len) res, \
        const __vec_typename(base, len) a, const __vec_typename(base, len) b,      \
        const __vec_typename(base, len) add)                                       \
    {                                                                              \
        for (u8 i = 0; i < len; i++)                                               \
        {                                                                          \
            res[i] = a[i] * b[i] + add[i];                                         \
        }                                                                          \
    }

#define __vec_madds(base, len)                                                          \
    MR_INLINE void v##len##base##_madds(__vec_typename(base, len) res,     \
        const __vec_typename(base, len) a, base b, const __vec_typename(base, len) add) \
    {                                                                                   \
        for (u8 i = 0; i < len; i++)                                                    \
        {                                                                               \
            res[i] = a[i] * b + add[i];                                                 \
        }                                                                               \
    }

#define __vec_maddss(base, len)                                                             \
    MR_INLINE void v##len##base##_maddss(                                      \
        __vec_typename(base, len) res, const __vec_typename(base, len) a, base b, base add) \
    {                                                                                       \
        for (u8 i = 0; i < len; i++)                                                        \
        {                                                                                   \
            res[i] = a[i] * b + add;                                                        \
        }                                                                                   \
    }

#define __vec_abs(base, len)                                                      \
    MR_INLINE void v##len##base##_abs(__vec_typename(base, len) vec) \
    {                                                                             \
        for (u8 i = 0; i < len; i++)                                              \
        {                                                                         \
            vec[i] = (vec[i] < 0) ? -vec[i] : vec[i];                             \
        }                                                                         \
    }

#define __vec_sign(base, len)                                                      \
    MR_INLINE void v##len##base##_sign(__vec_typename(base, len) vec) \
    {                                                                              \
        for (u8 i = 0; i < len; i++)                                               \
        {                                                                          \
            vec[i] = (vec[i] < 0) ? -1 : 1;                                        \
        }                                                                          \
    }

#define __vec_min(base, len)                                                      \
    MR_INLINE void v##len##base##_min(__vec_typename(base, len) res, \
        const __vec_typename(base, len) a, const __vec_typename(base, len) b)     \
    {                                                                             \
        for (u8 i = 0; i < len; i++)                                              \
        {                                                                         \
            res[i] = a[i] < b[i] ? a[i] : b[i];                                   \
        }                                                                         \
    }

#define __vec_max(base, len)                                                      \
    MR_INLINE void v##len##base##_max(__vec_typename(base, len) res, \
        const __vec_typename(base, len) a, const __vec_typename(base, len) b)     \
    {                                                                             \
        for (u8 i = 0; i < len; i++)                                              \
        {                                                                         \
            res[i] = a[i] > b[i] ? a[i] : b[i];                                   \
        }                                                                         \
    }

#define __vec_zeros(base, len)                                                      \
    MR_INLINE void v##len##base##_zeros(__vec_typename(base, len) vec) \
    {                                                                               \
        for (u8 i = 0; i < len; i++)                                                \
        {                                                                           \
            vec[i] = 0;                                                             \
        }                                                                           \
    }

#define __vec_ones(base, len)                                                      \
    MR_INLINE void v##len##base##_ones(__vec_typename(base, len) vec) \
    {                                                                              \
        for (u8 i = 0; i < len; i++)                                               \
        {                                                                          \
            vec[i] = 1;                                                            \
        }                                                                          \
    }

// ----------------------------------------------------------------------
// Real type implementations
// ----------------------------------------------------------------------

#define __real_vec_vec_op_impl(func, len)                                                   \
    MR_INLINE void v##len##_##func(v##len res, const v##len a, const v##len b) \
    {                                                                                       \
        for (u8 i = 0; i < len; i++)                                                        \
        {                                                                                   \
            res[i] = real_##func(a[i], b[i]);                                               \
        }                                                                                   \
    }

#define __real_vec_scal_op_impl(func, len)                                             \
    MR_INLINE void v##len##_##func##s(v##len res, const v##len a, real b) \
    {                                                                                  \
        for (u8 i = 0; i < len; i++)                                                   \
        {                                                                              \
            res[i] = real_##func(a[i], b);                                             \
        }                                                                              \
    }

#define __real_vec_inplace_op_impl(func, len)               \
    MR_INLINE void v##len##_##func(v##len vec) \
    {                                                       \
        for (u8 i = 0; i < len; i++)                        \
        {                                                   \
            vec[i] = real_##func(vec[i]);                   \
        }                                                   \
    }

#define __real_vec_madd(len)                                          \
    MR_INLINE void v##len##_madd(                        \
        v##len res, const v##len a, const v##len b, const v##len add) \
    {                                                                 \
        for (u8 i = 0; i < len; i++)                                  \
        {                                                             \
            res[i] = real_add(real_mul(a[i], b[i]), add[i]);          \
        }                                                             \
    }

#define __real_vec_madds(len)                                 \
    MR_INLINE void v##len##_madds(               \
        v##len res, const v##len a, real b, const v##len add) \
    {                                                         \
        for (u8 i = 0; i < len; i++)                          \
        {                                                     \
            res[i] = real_add(real_mul(a[i], b), add[i]);     \
        }                                                     \
    }

#define __real_vec_maddss(len)                                                                \
    MR_INLINE void v##len##_maddss(v##len res, const v##len a, real b, real add) \
    {                                                                                         \
        for (u8 i = 0; i < len; i++)                                                          \
        {                                                                                     \
            res[i] = real_add(real_mul(a[i], b), add);                                        \
        }                                                                                     \
    }

#define __real_vec_cpy(len)                                               \
    MR_INLINE void v##len##_cpy(v##len out, const v##len in) \
    {                                                                     \
        for (u8 i = 0; i < len; i++)                                      \
        {                                                                 \
            out[i] = in[i];                                               \
        }                                                                 \
    }

#define __real_vec_dot(len)                                                  \
    MR_INLINE real v##len##_dot(const v##len a, const v##len b) \
    {                                                                        \
        real res = REAL(0);                                                  \
        for (u8 i = 0; i < len; i++)                                         \
        {                                                                    \
            res = real_add(real_mul(a[i], b[i]), res);                       \
        }                                                                    \
        return res;                                                          \
    }

#define __real_vec_lerp(len)                                                                      \
    MR_INLINE void v##len##_lerp(v##len res, const v##len a, const v##len b, real f) \
    {                                                                                             \
        for (u8 i = 0; i < len; i++)                                                              \
        {                                                                                         \
            res[i] = real_add(real_mul(real_sub(REAL(1), f), a[i]), real_mul(f, b[i]));           \
        }                                                                                         \
    }

#define __real_vec_zeros(len)                              \
    MR_INLINE void v##len##_zeros(v##len out) \
    {                                                      \
        for (u8 i = 0; i < len; i++)                       \
        {                                                  \
            out[i] = REAL(0);                              \
        }                                                  \
    }

#define __real_vec_ones(len)                              \
    MR_INLINE void v##len##_ones(v##len out) \
    {                                                     \
        for (u8 i = 0; i < len; i++)                      \
        {                                                 \
            out[i] = REAL(1);                             \
        }                                                 \
    }

#define __real_vec_clamp(len)                                                  \
    MR_INLINE void v##len##_clamp(v##len vec, real min, real max) \
    {                                                                          \
        for (u8 i = 0; i < len; i++)                                           \
        {                                                                      \
            vec[i] = real_clamp(vec[i], min, max);                             \
        }                                                                      \
    }

#define __real_vec_dist(len)                                                  \
    MR_INLINE real v##len##_dist(const v##len a, const v##len b) \
    {                                                                         \
        real res = REAL(0);                                                   \
        for (u8 i = 0; i < len; i++)                                          \
        {                                                                     \
            const real t = real_sub(a[i], b[i]);                              \
            res = real_add(res, real_mul(t, t));                              \
        }                                                                     \
        return real_sqrt(res);                                                \
    }

#define __real_vec_length(len)                                  \
    MR_INLINE real v##len##_length(const v##len a) \
    {                                                           \
        real res = REAL(0);                                     \
        for (u8 i = 0; i < len; i++)                            \
        {                                                       \
            res = real_add(res, real_mul(a[i], a[i]));          \
        }                                                       \
        return real_sqrt(res);                                  \
    }

#define __real_vec_normalize(len)                              \
    MR_INLINE void v##len##_normalize(v##len vec) \
    {                                                          \
        v##len##_divs(vec, vec, v##len##_length(vec));         \
    }

// ----------------------------------------------------------------------
// Operation groups
// ----------------------------------------------------------------------

#define __vec_arithmetic_op_impl(base, len) \
    __vec_vec_op_impl(add, +, base, len);   \
    __vec_vec_op_impl(sub, -, base, len);   \
    __vec_vec_op_impl(mul, *, base, len);   \
    __vec_vec_op_impl(div, /, base, len);   \
    __vec_scal_op_impl(add, +, base, len);  \
    __vec_scal_op_impl(sub, -, base, len);  \
    __vec_scal_op_impl(mul, *, base, len);  \
    __vec_scal_op_impl(div, /, base, len);

#define __vec_mod_op_impl(base, len)      \
    __vec_vec_op_impl(mod, %, base, len); \
    __vec_scal_op_impl(mod, %, base, len);

#define __real_vec_impl(len)               \
    __real_vec_vec_op_impl(add, len);      \
    __real_vec_vec_op_impl(sub, len);      \
    __real_vec_vec_op_impl(mul, len);      \
    __real_vec_vec_op_impl(div, len);      \
    __real_vec_vec_op_impl(min, len);      \
    __real_vec_vec_op_impl(max, len);      \
    __real_vec_scal_op_impl(add, len);     \
    __real_vec_scal_op_impl(sub, len);     \
    __real_vec_scal_op_impl(mul, len);     \
    __real_vec_scal_op_impl(div, len);     \
    __real_vec_inplace_op_impl(abs, len);  \
    __real_vec_inplace_op_impl(inv, len);  \
    __real_vec_inplace_op_impl(sign, len); \
    __real_vec_madd(len);                  \
    __real_vec_madds(len);                 \
    __real_vec_maddss(len);                \
    __real_vec_cpy(len);                   \
    __real_vec_dot(len);                   \
    __real_vec_lerp(len);                  \
    __real_vec_zeros(len);                 \
    __real_vec_ones(len);                  \
    __real_vec_clamp(len);                 \
    __real_vec_dist(len);                  \
    __real_vec_length(len);                \
    __real_vec_normalize(len);

// ----------------------------------------------------------------------
// Per type groups
// ----------------------------------------------------------------------

#define __vec_common_type_define_impl(base, len) \
    __vec_arithmetic_op_impl(base, len);         \
    __vec_mod_op_impl(base, len);                \
    __vec_madd(base, len);                       \
    __vec_madds(base, len);                      \
    __vec_maddss(base, len);                     \
    __vec_cpy(base, len);                        \
    __vec_min(base, len);                        \
    __vec_max(base, len);                        \
    __vec_zeros(base, len);                      \
    __vec_ones(base, len);                       \
    __vec_clamp(base, len);

#define __vec_int_type_define_impl(base, len) \
    __vec_common_type_define_impl(base, len); \
    __vec_abs(base, len);                     \
    __vec_sign(base, len);

#define __vec_uint_type_define_impl(base, len) __vec_common_type_define_impl(base, len);

// ----------------------------------------------------------------------
// Macro "calls"
// ----------------------------------------------------------------------

__vec_typedef(__vec_int_type_define_impl, i8);
__vec_typedef(__vec_int_type_define_impl, i16);
__vec_typedef(__vec_int_type_define_impl, i32);
__vec_typedef(__vec_uint_type_define_impl, u8);
__vec_typedef(__vec_uint_type_define_impl, u16);
__vec_typedef(__vec_uint_type_define_impl, u32);

__real_vec_impl(2);
__real_vec_impl(3);
__real_vec_impl(4);

// ----------------------------------------------------------------------
// Specific functions
// ----------------------------------------------------------------------

MR_INLINE void v3_cross(v3 res, const v3 a, const v3 b)
{
    res[0] = real_sub(real_mul(a[1], b[2]), real_mul(a[2], b[1]));
    res[1] = real_sub(real_mul(a[2], b[0]), real_mul(a[0], b[2]));
    res[2] = real_sub(real_mul(a[0], b[1]), real_mul(a[1], b[0]));
}

#endif
