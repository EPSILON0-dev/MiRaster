#ifndef MR_TYPES_H
#define MR_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "mr/config.h"

// ----------------------------------------------------------------------
// Value types
// ----------------------------------------------------------------------

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

#if defined(MR_REAL_MODE_FLOATING_POINT)
typedef float real;
#elif defined(MR_REAL_MODE_FIXED_POINT)
#error "Invalid real type"
#else
#error "Invalid real type"
#endif

// --------------vec--------------------------------------------------------
// Enums
// ----------------------------------------------------------------------

enum mr_result
{
    MR_OK = 0,
    MR_ERROR,
    MR_UNIMP,
    MR_INVALID_FORMAT,
    MR_INVALID_TYPE,
    MR_MODIFIED,
    MR_OUT_OF_RANGE,
    MR_UNREACHABLE,
    MR_NOMEM,
    MR_NOWAIT,
};

typedef int mr_result_t;

// ----------------------------------------------------------------------
// Type definitions
// ----------------------------------------------------------------------

#define __vec_typename(base, len) v##len##base
#define __vec_v_opname(name, base, len) v##len##base##_##name
#define __vec_s_opname(name, base, len) v##len##base##_##name##s
#define __vec_typedecl(base, len) typedef base __vec_typename(base, len)[len];
#define __real_vec_typedecl(len) typedef real v##len[len];

#define __mat_typename(base, size) mat##size##base
#define __mat_typedecl(base, size) typedef base __mat_typename(base, size)[size * size];
#define __real_mat_typedecl(len) typedef real mat##len[len * len];

#define __vec_typedef(X, base) \
    X(base, 2);                \
    X(base, 3);                \
    X(base, 4);

#define __real_typedef(X) \
    X(2);                 \
    X(3);                 \
    X(4);

// ----------------------------------------------------------------------
// Macro "calls"
// ----------------------------------------------------------------------

__vec_typedef(__vec_typedecl, i8);
__vec_typedef(__vec_typedecl, u8);
__vec_typedef(__vec_typedecl, i16);
__vec_typedef(__vec_typedecl, u16);
__vec_typedef(__vec_typedecl, i32);
__vec_typedef(__vec_typedecl, u32);

__real_typedef(__real_vec_typedecl);
__real_typedef(__real_mat_typedecl);

#endif
