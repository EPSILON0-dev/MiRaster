#ifndef MR_REAL_MATH_IMPL_H
#define MR_REAL_MATH_IMPL_H

#include <math.h>

#include "mr/config.h"
#include "mr/types.h"

#ifdef MR_REAL_MODE_FLOATING_POINT

#define REAL(x) ((float)x)

#define __real_op_impl(name, operator)                                  \
    MR_FORCE_STATIC_INLINE real real_##name(const real a, const real b) \
    {                                                                   \
        return a operator b;                                            \
    }

#define __real_xform(name, xform)                     \
    MR_FORCE_STATIC_INLINE real real_##name(real val) \
    {                                                 \
        return xform;                                 \
    }

#define __real_cmp(name, operator)                              \
    MR_FORCE_STATIC_INLINE bool real_##name(real lhs, real rhs) \
    {                                                           \
        return lhs operator rhs;                                \
    }

__real_op_impl(add, +);
__real_op_impl(sub, -);
__real_op_impl(mul, *);
__real_op_impl(div, /);

__real_xform(abs, (val < 0 ? -val : val));
__real_xform(sign, (val < 0 ? -1 : 1));
__real_xform(inv, (-val));
__real_xform(sqrt, (sqrtf(val)));
__real_xform(sin, (sinf(val)));
__real_xform(cos, (cosf(val)));
__real_xform(tan, (tanf(val)));
__real_xform(fract, (fabsf(val - truncf(val))));

__real_cmp(lt, <);
__real_cmp(gt, >);
__real_cmp(le, <=);
__real_cmp(ge, >=);
__real_cmp(ne, !=);
__real_cmp(eq, ==);

MR_FORCE_STATIC_INLINE real real_clamp(real val, real min, real max)
{
    return (val < min) ? min : (val > max) ? max : val;
}

MR_FORCE_STATIC_INLINE real real_min(real a, real b)
{
    return a < b ? a : b;
}

MR_FORCE_STATIC_INLINE real real_max(real a, real b)
{
    return a > b ? a : b;
}

MR_FORCE_STATIC_INLINE bool real_is_zero(real val)
{
    return !val;
}

MR_FORCE_STATIC_INLINE bool real_is_non_zero(real val)
{
    return !!val;
}

MR_FORCE_STATIC_INLINE void mr_color_real_to_u8(v4u8 out, v4 in)
{
    for (u8 i = 0; i < 4; i++)
    {
        out[i] = (u8)(real_clamp(in[i], 0.0f, 1.0f) * 255.0f);
    }
}

MR_FORCE_STATIC_INLINE void mr_color_u8_to_real(v4 out, v4u8 in)
{
    for (u8 i = 0; i < 4; i++)
    {
        out[i] = (real)(in[i]) / 255.0f;
    }
}

#define MR_RAD2DEG(x) ((x) * (180.0f / (real)M_PI))
#define MR_DEG2RAD(x) ((x) * ((real)M_PI / 180.0f))

#endif

#endif
