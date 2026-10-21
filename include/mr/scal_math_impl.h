#ifndef MR_SCAL_MATH_IMPL_H
#define MR_SCAL_MATH_IMPL_H

#define __scal_abs(type)                             \
    MR_FORCE_STATIC_INLINE type type##_abs(type val) \
    {                                                \
        return val < 0 ? -val : val;                 \
    }

#define __scal_clamp(type)                                                 \
    MR_FORCE_STATIC_INLINE type type##_clamp(type val, type min, type max) \
    {                                                                      \
        return (val < min) ? min : (val > max) ? max : val;                \
    }

#endif
