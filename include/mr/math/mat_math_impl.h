#ifndef MR_MAT_MATH_IMPL_H
#define MR_MAT_MATH_IMPL_H

#include <assert.h>  // IWYU pragma: keep

#include "mr/math/real_math_impl.h"
#include "mr/math/vec_math_impl.h"

// ----------------------------------------------------------------------
// Implementations
// ----------------------------------------------------------------------

#define __mat_op_mulvec(size)                                             \
    MR_INLINE void mat##size##_mulvec(                       \
        v##size res, const mat##size mat, const v##size vec)              \
    {                                                                     \
        v##size tmp;                                                      \
        v##size##_cpy(tmp, vec);                                          \
        for (u8 i = 0; i < size; i++)                                     \
        {                                                                 \
            real sum = REAL(0);                                           \
            for (u8 j = 0; j < size; j++)                                 \
            {                                                             \
                sum = real_add(sum, real_mul(mat[size * j + i], tmp[j])); \
            }                                                             \
            res[i] = sum;                                                 \
        }                                                                 \
    }

#define __mat_op_mulmat(size)                                    \
    MR_INLINE void mat##size##_mulmat(              \
        mat##size res, const mat##size a, const mat##size b)     \
    {                                                            \
        for (u8 i = 0; i < size; i++)                            \
        {                                                        \
            mat##size##_mulvec(&res[i * size], a, &b[i * size]); \
        }                                                        \
    }

#define __mat_op_transpose(size)                                                         \
    MR_INLINE void mat##size##_transpose(mat##size out, const mat##size in) \
    {                                                                                    \
        assert(in != out);                                                               \
        for (u8 i = 0; i < size; i++)                                                    \
        {                                                                                \
            for (u8 j = 0; j < size; j++)                                                \
            {                                                                            \
                out[i * size + j] = in[j * size + i];                                    \
            }                                                                            \
        }                                                                                \
    }

#define __mat_op_cpy(size)                                                         \
    MR_INLINE void mat##size##_cpy(mat##size out, const mat##size in) \
    {                                                                              \
        assert(in != out);                                                         \
        for (u8 i = 0; i < size * size; i++)                                       \
        {                                                                          \
            out[i] = in[i];                                                        \
        }                                                                          \
    }

// ----------------------------------------------------------------------
// Operation groups
// ----------------------------------------------------------------------

#define __mat_type_define_impl(size) \
    __mat_op_transpose(size);        \
    __mat_op_cpy(size);              \
    __mat_op_mulvec(size);           \
    __mat_op_mulmat(size);

#define __mat_typedef(X) \
    X(2);                \
    X(3);                \
    X(4);

// ----------------------------------------------------------------------
// Macro "calls"
// ----------------------------------------------------------------------

__mat_typedef(__mat_type_define_impl);

#endif
