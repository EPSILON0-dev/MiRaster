#ifndef MR_MATH_H
#define MR_MATH_H

// ----------------------------------------------------------------------
// Real type implementations
// ----------------------------------------------------------------------

#include "mr/math/real_math_impl.h"  // IWYU pragma: keep

// ----------------------------------------------------------------------
// Scalar type implementations
// ----------------------------------------------------------------------

#include "mr/math/scal_math_impl.h"  // IWYU pragma: keep

// ----------------------------------------------------------------------
// Vector type implementations
// ----------------------------------------------------------------------

#include "mr/math/vec_math_impl.h"  // IWYU pragma: keep

// ----------------------------------------------------------------------
// Matrix type implementations
// ----------------------------------------------------------------------

#include "mr/math/mat_math_impl.h"  // IWYU pragma: keep

// ----------------------------------------------------------------------
// Other/Inter-type implementations
// ----------------------------------------------------------------------

#if defined(MR_REAL_MODE_FLOATING_POINT)

MR_INLINE real int2real(i32 val)
{
    return (real)val;
}

MR_INLINE real real2int(real val)
{
    return (i32)val;
}

#elif defined(MR_REAL_MODE_FIXED_POINT)
#error "Invalid real type"
#else
#error "Invalid real type"
#endif

#endif
