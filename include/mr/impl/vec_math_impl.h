#ifndef MR_VEC_MATH_IMPL_H
#define MR_VEC_MATH_IMPL_H

#include <math.h>  // IWYU pragma: keep

#include "mr/config.h"

// ----------------------------------------------------------------------
// Implementations
// ----------------------------------------------------------------------

#define __vec_vec_op_impl(name, operator, base, len)                                           \
    MR_FORCE_STATIC_INLINE void __vec_v_opname(name, base, len)(__vec_typename(base, len) res, \
        const __vec_typename(base, len) a, const __vec_typename(base, len) b)                  \
    {                                                                                          \
        for (u8 i = 0; i < len; i++)                                                           \
        {                                                                                      \
            res[i] = a[i] operator b[i];                                                       \
        }                                                                                      \
    }

#define __vec_scal_op_impl(name, operator, base, len)                                   \
    MR_FORCE_STATIC_INLINE void __vec_s_opname(name, base, len)(                        \
        __vec_typename(base, len) res, const __vec_typename(base, len) a, const base b) \
    {                                                                                   \
        for (u8 i = 0; i < len; i++)                                                    \
        {                                                                               \
            res[i] = a[i] operator b;                                                   \
        }                                                                               \
    }

#define __vec_clamp(base, len)                                             \
    MR_FORCE_STATIC_INLINE void v##len##base##_clamp(                      \
        __vec_typename(base, len) val, const base min, const base max)     \
    {                                                                      \
        for (u8 i = 0; i < len; i++)                                       \
        {                                                                  \
            val[i] = (val[i] < min) ? min : (val[i] > max) ? max : val[i]; \
        }                                                                  \
    }

#define __vec_dot(base, len)                                                  \
    MR_FORCE_STATIC_INLINE base v##len##base##_dot(                           \
        const __vec_typename(base, len) a, const __vec_typename(base, len) b) \
    {                                                                         \
        base res = 0;                                                         \
        for (u8 i = 0; i < len; i++)                                          \
        {                                                                     \
            res += a[i] * b[i];                                               \
        }                                                                     \
        return res;                                                           \
    }

#define __vec_lerp(base, len)                                                               \
    MR_FORCE_STATIC_INLINE void v##len##base##_lerp(__vec_typename(base, len) res,          \
        const __vec_typename(base, len) a, const __vec_typename(base, len) b, const base f) \
    {                                                                                       \
        for (u8 i = 0; i < len; i++)                                                        \
        {                                                                                   \
            res[i] = (1 - f) * a[i] + f * b[i];                                             \
        }                                                                                   \
    }

#define __vec_cpy(base, len)                                                \
    MR_FORCE_STATIC_INLINE void v##len##base##_cpy(                         \
        __vec_typename(base, len) dst, const __vec_typename(base, len) src) \
    {                                                                       \
        for (u8 i = 0; i < len; i++)                                        \
        {                                                                   \
            dst[i] = src[i];                                                \
        }                                                                   \
    }

#define __vec_madd(base, len)                                                      \
    MR_FORCE_STATIC_INLINE void v##len##base##_madd(__vec_typename(base, len) res, \
        const __vec_typename(base, len) a, const __vec_typename(base, len) b,      \
        const __vec_typename(base, len) add)                                       \
    {                                                                              \
        for (u8 i = 0; i < len; i++)                                               \
        {                                                                          \
            res[i] = a[i] * b[i] + add[i];                                         \
        }                                                                          \
    }

#define __vec_madds(base, len)                                                          \
    MR_FORCE_STATIC_INLINE void v##len##base##_madds(__vec_typename(base, len) res,     \
        const __vec_typename(base, len) a, base b, const __vec_typename(base, len) add) \
    {                                                                                   \
        for (u8 i = 0; i < len; i++)                                                    \
        {                                                                               \
            res[i] = a[i] * b + add[i];                                                 \
        }                                                                               \
    }

#define __vec_abs(base, len)                                                      \
    MR_FORCE_STATIC_INLINE void v##len##base##_abs(__vec_typename(base, len) vec) \
    {                                                                             \
        for (u8 i = 0; i < len; i++)                                              \
        {                                                                         \
            vec[i] = (vec[i] < 0) ? -vec[i] : vec[i];                             \
        }                                                                         \
    }

#define __vec_sign(base, len)                                                      \
    MR_FORCE_STATIC_INLINE void v##len##base##_sign(__vec_typename(base, len) vec) \
    {                                                                              \
        for (u8 i = 0; i < len; i++)                                               \
        {                                                                          \
            vec[i] = (vec[i] < 0) ? -1 : 1;                                        \
        }                                                                          \
    }

#define __vec_min(base, len)                                                      \
    MR_FORCE_STATIC_INLINE void v##len##base##_min(__vec_typename(base, len) res, \
        const __vec_typename(base, len) a, const __vec_typename(base, len) b)     \
    {                                                                             \
        for (u8 i = 0; i < len; i++)                                              \
        {                                                                         \
            res[i] = a[i] < b[i] ? a[i] : b[i];                                   \
        }                                                                         \
    }

#define __vec_max(base, len)                                                      \
    MR_FORCE_STATIC_INLINE void v##len##base##_max(__vec_typename(base, len) res, \
        const __vec_typename(base, len) a, const __vec_typename(base, len) b)     \
    {                                                                             \
        for (u8 i = 0; i < len; i++)                                              \
        {                                                                         \
            res[i] = a[i] > b[i] ? a[i] : b[i];                                   \
        }                                                                         \
    }

#define __vec_zeros(base, len)                                                      \
    MR_FORCE_STATIC_INLINE void v##len##base##_zeros(__vec_typename(base, len) vec) \
    {                                                                               \
        for (u8 i = 0; i < len; i++)                                                \
        {                                                                           \
            vec[i] = 0;                                                             \
        }                                                                           \
    }

#define __vec_ones(base, len)                                                      \
    MR_FORCE_STATIC_INLINE void v##len##base##_ones(__vec_typename(base, len) vec) \
    {                                                                              \
        for (u8 i = 0; i < len; i++)                                               \
        {                                                                          \
            vec[i] = 1;                                                            \
        }                                                                          \
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

// ----------------------------------------------------------------------
// Per type groups
// ----------------------------------------------------------------------

#define __vec_common_type_define_impl(base, len) \
    __vec_arithmetic_op_impl(base, len);         \
    __vec_madd(base, len);                       \
    __vec_madds(base, len);                      \
    __vec_cpy(base, len);                        \
    __vec_min(base, len);                        \
    __vec_max(base, len);                        \
    __vec_zeros(base, len);                      \
    __vec_ones(base, len);                       \
    __vec_clamp(base, len);

#define __vec_float_type_define_impl(base, len) \
    __vec_common_type_define_impl(base, len);   \
    __vec_dot(base, len);                       \
    __vec_abs(base, len);                       \
    __vec_sign(base, len);                      \
    __vec_lerp(base, len);

#define __vec_int_type_define_impl(base, len) \
    __vec_common_type_define_impl(base, len); \
    __vec_mod_op_impl(base, len);             \
    __vec_abs(base, len);                     \
    __vec_sign(base, len);

#define __vec_uint_type_define_impl(base, len) \
    __vec_common_type_define_impl(base, len);  \
    __vec_mod_op_impl(base, len);

// ----------------------------------------------------------------------
// Type specific operations
// ----------------------------------------------------------------------

#define __vec_f32_dist_impl(len)                                                          \
    MR_FORCE_STATIC_INLINE f32 v##len##f32_dist(const v##len##f32 a, const v##len##f32 b) \
    {                                                                                     \
        f32 sum = 0.0f;                                                                   \
        for (u8 i = 0; i < len; i++)                                                      \
        {                                                                                 \
            const f32 diff = (a[i] - b[i]);                                               \
            sum += diff * diff;                                                           \
        }                                                                                 \
        return sqrtf(sum);                                                                \
    }

#endif
