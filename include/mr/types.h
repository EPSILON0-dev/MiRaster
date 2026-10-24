#ifndef MR_TYPES_H
#define MR_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// ----------------------------------------------------------------------
// Value types
// ----------------------------------------------------------------------

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

// ----------------------------------------------------------------------
// Enums
// ----------------------------------------------------------------------

enum mr_result
{
    MR_OK = 0,
    MR_ERROR,
    MR_UNIMP,
    MR_INVALID_FORMAT,
    MR_MODIFIED,
    MR_OUT_OF_RANGE,
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

#define __mat_typename(base, size) mat##size##base
#define __mat_typedecl(base, size) typedef base __mat_typename(base, size)[size * size];

#define __vec_typedef(X, base) \
    X(base, 2);                \
    X(base, 3);                \
    X(base, 4);

#define __mat_typedef(X, base) \
    X(base, 2);                \
    X(base, 3);                \
    X(base, 4);

// ----------------------------------------------------------------------
// Macro "calls"
// ----------------------------------------------------------------------

__vec_typedef(__vec_typedecl, f32);
__vec_typedef(__vec_typedecl, f64);
__vec_typedef(__vec_typedecl, i8);
__vec_typedef(__vec_typedecl, u8);
__vec_typedef(__vec_typedecl, i16);
__vec_typedef(__vec_typedecl, u16);
__vec_typedef(__vec_typedecl, i32);
__vec_typedef(__vec_typedecl, u32);
__vec_typedef(__vec_typedecl, i64);
__vec_typedef(__vec_typedecl, u64);

__mat_typedef(__mat_typedecl, f32);
__mat_typedef(__mat_typedecl, f64);

#endif
