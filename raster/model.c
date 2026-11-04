#include "mr/model.h"

#include "mr/math.h"  // IWYU pragma: keep
#include "mr/matrix.h"
#include "mr/render.h"
#include "mr/types.h"

void mr_model_matrix(mat4 matrix, const struct mr_model* model)
{
    mat4 translation, rotation_x, rotation_y, rotation_z, scale;
    mat4 temp0, temp1;

    mat4_translate(translation, model->translation);
    mat4_rotate_around_x(rotation_x, model->rotation[0]);
    mat4_rotate_around_y(rotation_y, model->rotation[1]);
    mat4_rotate_around_z(rotation_z, model->rotation[2]);
    mat4_scale(scale, model->scale);

    mat4_mulmat(temp0, rotation_x, rotation_y);
    mat4_mulmat(temp1, temp0, rotation_z);
    mat4_mulmat(temp0, translation, temp1);
    mat4_mulmat(matrix, temp0, scale);
}

MR_INLINE void mr_model_transform_vertex(v4 outvert, const mat4 mvp, const void* invert)
{
    v4 vert;
    v3_cpy(vert, invert);
    vert[3] = 1.0f;

    mat4_mulvec(outvert, mvp, vert);
    real w = outvert[3];
    outvert[3] = 1.0f;
    v4_divs(outvert, outvert, real_abs(w));
}

MR_INLINE mr_result_t mr_model_draw_lines(
    struct mr_fb_desc* fb, const struct mr_model* model, const mat4 mvp)
{
    v4 line[2];
    struct mr_line_draw_cmd cmd = {
        .fb = fb,
        .verts = line,
        .shader = model->shader_func,
        .data = model->shader_data,
    };

    for (size_t i = 0; i < model->mesh.num_indices; i += 2)
    {
        for (u8 j = 0; j < 2; j++)
        {
            const size_t offset = (model->mesh.indices[i + j] * model->mesh.stride);
            const void* ptr = (u8*)model->mesh.vertices + offset;
            mr_model_transform_vertex(line[j], mvp, ptr);
        }
        mr_draw_line(&cmd);
    }

    return MR_OK;
}

MR_INLINE mr_result_t mr_model_draw_triangles(
    struct mr_fb_desc* fb, const struct mr_model* model, const mat4 mvp)
{
    v4 tri[3];
    struct mr_triangle_draw_cmd cmd = {
        .fb = fb,
        .verts = tri,
        .shader = model->shader_func,
        .data = model->shader_data,
    };

    for (size_t i = 0; i < model->mesh.num_indices; i += 3)
    {
        for (u8 j = 0; j < 3; j++)
        {
            const size_t offset = (model->mesh.indices[i + j] * model->mesh.stride);
            const void* ptr = (u8*)model->mesh.vertices + offset;
            cmd.vert_ptrs[j] = ptr;
            mr_model_transform_vertex(tri[j], mvp, ptr);
        }
        mr_draw_triangle(&cmd);
    }

    return MR_OK;
}

mr_result_t mr_model_draw(struct mr_fb_desc* fb, const struct mr_model* model, const mat4 cam_vp)
{
    // Compute the MVP matrix
    mat4 model_mat, mvp;
    mr_model_matrix(model_mat, model);
    mat4_mulmat(mvp, cam_vp, model_mat);

    // Mesh sanity checks
    if (model->mesh.stride % sizeof(real) != 0) return MR_INVALID_FORMAT;

    // Select the draw type and... draw
    switch (model->mesh.type)
    {
        case MR_MODEL_LINES:
            return mr_model_draw_lines(fb, model, mvp);

        case MR_MODEL_TRIANGLES:
            return mr_model_draw_triangles(fb, model, mvp);

        default:
            return MR_INVALID_TYPE;
    }

    return MR_UNREACHABLE;
}
