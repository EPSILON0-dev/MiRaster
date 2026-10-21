#ifndef MR_MAT_MATH_IMPL_H
#define MR_MAT_MATH_IMPL_H

#include <assert.h>  // IWYU pragma: keep

#include "mr/config.h"

// ----------------------------------------------------------------------
// Implementations
// ----------------------------------------------------------------------

#define __mat_op_mulvec(base, size)                                                      \
    MR_FORCE_STATIC_INLINE void mat##size##base##_mulvec(__vec_typename(base, size) res, \
        const __mat_typename(base, size) mat, const __vec_typename(base, size) vec)      \
    {                                                                                    \
        v##size##base tmp;                                                               \
        v##size##base##_cpy(tmp, vec);                                                   \
        for (u8 i = 0; i < size; i++)                                                    \
        {                                                                                \
            base sum = 0;                                                                \
            for (u8 j = 0; j < size; j++)                                                \
            {                                                                            \
                sum += mat[size * j + i] * tmp[j];                                       \
            }                                                                            \
            res[i] = sum;                                                                \
        }                                                                                \
    }

#define __mat_op_mulmat(base, size)                                                      \
    MR_FORCE_STATIC_INLINE void mat##size##base##_mulmat(__mat_typename(base, size) res, \
        const __mat_typename(base, size) a, const __mat_typename(base, size) b)          \
    {                                                                                    \
        for (u8 i = 0; i < size; i++)                                                    \
        {                                                                                \
            mat##size##base##_mulvec(&res[i * size], a, &b[i * size]);                   \
        }                                                                                \
    }

#define __mat_op_transpose(base, size)                                       \
    MR_FORCE_STATIC_INLINE void mat##size##base##_transpose(                 \
        __mat_typename(base, size) out, const __mat_typename(base, size) in) \
    {                                                                        \
        assert(in != out);                                                   \
        for (u8 i = 0; i < size; i++)                                        \
        {                                                                    \
            for (u8 j = 0; j < size; j++)                                    \
            {                                                                \
                out[i * size + j] = in[j * size + i];                        \
            }                                                                \
        }                                                                    \
    }

#define __mat_op_cpy(base, size)                                             \
    MR_FORCE_STATIC_INLINE void mat##size##base##_cpy(                       \
        __mat_typename(base, size) out, const __mat_typename(base, size) in) \
    {                                                                        \
        assert(in != out);                                                   \
        for (u8 i = 0; i < size * size; i++)                                 \
        {                                                                    \
            out[i] = in[i];                                                  \
        }                                                                    \
    }

// ----------------------------------------------------------------------
// Operation groups
// ----------------------------------------------------------------------

#define __mat_type_define_impl(base, size) \
    __mat_op_transpose(base, size);        \
    __mat_op_cpy(base, size);              \
    __mat_op_mulvec(base, size);           \
    __mat_op_mulmat(base, size);

#define __mat_typedef(X, base) \
    X(base, 2);                \
    X(base, 3);                \
    X(base, 4);

#endif
