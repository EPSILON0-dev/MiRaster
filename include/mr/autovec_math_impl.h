#ifndef MR_AUTOVEC_MATH_IMPL_H
#define MR_AUTOVEC_MATH_IMPL_H

#include "mr/config.h"

// ----------------------------------------------------------------------
// Implementations
// ----------------------------------------------------------------------

#define __vec_vec_op_impl(name, operator, base, len)                                           \
    MR_FORCE_STATIC_INLINE void __vec_v_opname(name, base, len)(__vec_typename(base, len) res, \
        const __vec_typename(base, len) a, const __vec_typename(base, len) b)                  \
    {                                                                                          \
        for (uint8_t i = 0; i < len; i++)                                                      \
        {                                                                                      \
            res[i] = a[i] operator b[i];                                                       \
        }                                                                                      \
    }

#define __vec_scal_op_impl(name, operator, base, len)                                   \
    MR_FORCE_STATIC_INLINE void __vec_s_opname(name, base, len)(                        \
        __vec_typename(base, len) res, const __vec_typename(base, len) a, const base b) \
    {                                                                                   \
        for (uint8_t i = 0; i < len; i++)                                               \
        {                                                                               \
            res[i] = a[i] operator b;                                                   \
        }                                                                               \
    }

#define __vec_clamp(base, len)                                             \
    MR_FORCE_STATIC_INLINE void v##len##base##_clamp(                      \
        __vec_typename(base, len) val, const base min, const base max)     \
    {                                                                      \
        for (uint8_t i = 0; i < len; i++)                                  \
        {                                                                  \
            val[i] = (val[i] < min) ? min : (val[i] > max) ? max : val[i]; \
        }                                                                  \
    }

#define __vec_dot(base, len)                                                  \
    MR_FORCE_STATIC_INLINE base v##len##base##_dot(                           \
        const __vec_typename(base, len) a, const __vec_typename(base, len) b) \
    {                                                                         \
        base res = 0;                                                         \
        for (uint8_t i = 0; i < len; i++)                                     \
        {                                                                     \
            res += a[i] * b[i];                                               \
        }                                                                     \
        return res;                                                           \
    }

#define __vec_lerp(base, len)                                                               \
    MR_FORCE_STATIC_INLINE void v##len##base##_lerp(__vec_typename(base, len) res,          \
        const __vec_typename(base, len) a, const __vec_typename(base, len) b, const base f) \
    {                                                                                       \
        for (uint8_t i = 0; i < len; i++)                                                   \
        {                                                                                   \
            res[i] = (1 - f) * a[i] + f * b[i];                                             \
        }                                                                                   \
    }

#define __vec_cpy(base, len)                                                \
    MR_FORCE_STATIC_INLINE void v##len##base##_cpy(                         \
        __vec_typename(base, len) dst, const __vec_typename(base, len) src) \
    {                                                                       \
        for (uint8_t i = 0; i < len; i++)                                   \
        {                                                                   \
            dst[i] = src[i];                                                \
        }                                                                   \
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

#define __vec_float_type_define_impl(base, len) \
    __vec_arithmetic_op_impl(base, len);        \
    __vec_clamp(base, len);                     \
    __vec_dot(base, len);                       \
    __vec_cpy(base, len);                       \
    __vec_lerp(base, len);

#define __vec_int_type_define_impl(base, len) \
    __vec_arithmetic_op_impl(base, len);      \
    __vec_mod_op_impl(base, len);             \
    __vec_cpy(base, len);                     \
    __vec_clamp(base, len);

#endif
