#include "mr/shader.h"

#include "mr/math.h"  // IWYU pragma: keep

MR_INLINE void mr_triangle_shader_normalize_barycoord(
    v3 barycoord, const struct mr_triangle_shader_inputs* inputs)
{
    v3 w_vec = {
        inputs->cmd->verts[0][3],  //
        inputs->cmd->verts[1][3],  //
        inputs->cmd->verts[2][3],  //
    };
    v3_mul(barycoord, inputs->barycoord, w_vec);

    const real sum = real_add(real_add(barycoord[0], barycoord[1]), barycoord[2]);
    v3_muls(barycoord, barycoord, 1.0f / sum);
}

MR_INLINE real mr_shader_interpolate_real(const void** vert_ptrs, v3 barycoord, size_t index)
{
    const size_t offset = index * sizeof(real);
    real* ptrs[3];

    for (u8 i = 0; i < 3; i++)
    {
        ptrs[i] = (real*)((u8*)(vert_ptrs[i]) + offset);
    }

    real res;
    res = real_mul(*(ptrs[0]), barycoord[0]);
    res = real_add(real_mul(*(ptrs[1]), barycoord[1]), res);
    res = real_add(real_mul(*(ptrs[2]), barycoord[2]), res);
    return res;
}

MR_INLINE void mr_shader_interpolate_all(
    real* res, const void** vert_ptrs, v3 barycoord, size_t count)
{
    for (u8 i = 0; i < count; i++)
    {
        res[i] = mr_shader_interpolate_real(vert_ptrs, barycoord, i);
    }
}

void mr_shader_point_data(void* res, const struct mr_triangle_shader_inputs* inputs, size_t count)
{
    v3 barycoord;
    mr_triangle_shader_normalize_barycoord(barycoord, inputs);

    mr_shader_interpolate_all((real*)res,         //
        (const void**)(&inputs->cmd->vert_ptrs),  //
        barycoord,                                //
        count                                     //
    );
}
