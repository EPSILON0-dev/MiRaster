#include "mr/model.h"

#include "mr/config.h"
#include "mr/math.h"
#include "mr/matrix.h"
#include "mr/render.h"
#include "mr/types.h"

void mr_model_matrix(mat4f32 matrix, const struct mr_model* model)
{
    mat4f32 translation, rotation_x, rotation_y, rotation_z, scale;
    mat4f32 temp0, temp1;

    mat4f32_translate(translation, model->translation);
    mat4f32_rotate_around_x(rotation_x, model->rotation[0]);
    mat4f32_rotate_around_y(rotation_y, model->rotation[1]);
    mat4f32_rotate_around_z(rotation_z, model->rotation[2]);
    mat4f32_scale(scale, model->scale);

    mat4f32_mulmat(temp0, rotation_x, rotation_y);
    mat4f32_mulmat(temp1, temp0, rotation_z);
    mat4f32_mulmat(temp0, translation, temp1);
    mat4f32_mulmat(matrix, temp0, scale);
}

MR_FORCE_STATIC_INLINE void mr_model_transform_vertex(
    v4f32 outvert, const struct mr_model* model, const mat4f32 mvp, u32 vertindex)
{
    v4f32 vert;
    const u32 stride = (model->mesh.stride / sizeof(f32));
    const u32 index = (u32)(model->mesh.indices[vertindex]) * stride;
    v3f32_cpy(vert, &model->mesh.vertices[index]);
    vert[3] = 1.0f;

    mat4f32_mulvec(outvert, mvp, vert);
    f32 w = outvert[3];
    outvert[3] = 1.0f;
    v4f32_divs(outvert, outvert, f32_abs(w));
}

MR_FORCE_STATIC_INLINE mr_result_t mr_model_draw_lines(
    struct mr_fb_desc* fb, const struct mr_model* model, const mat4f32 mvp)
{
    v4f32 line[2];
    struct mr_line_draw_cmd cmd = {
        .fb = fb,
        .verts = line,
        .shader = model->shader_func,
        .data = model->shader_data,
    };

    for (size_t i = 0; i < model->mesh.num_indices; i += 2)
    {
        mr_model_transform_vertex(line[0], model, mvp, (i + 0));
        mr_model_transform_vertex(line[1], model, mvp, (i + 1));
        mr_draw_line(&cmd);
    }

    return MR_OK;
}

MR_FORCE_STATIC_INLINE mr_result_t mr_model_draw_triangles(
    struct mr_fb_desc* fb, const struct mr_model* model, const mat4f32 mvp)
{
    v4f32 tri[3];
    struct mr_triangle_draw_cmd cmd = {
        .fb = fb,
        .verts = tri,
        .inds = NULL,
        .stride = model->mesh.stride,
        .shader = model->shader_func,
        .data = model->shader_data,
    };

    for (size_t i = 0; i < model->mesh.num_indices; i += 3)
    {
        cmd.inds = &model->mesh.indices[i];
        mr_model_transform_vertex(tri[0], model, mvp, (i + 0));
        mr_model_transform_vertex(tri[1], model, mvp, (i + 1));
        mr_model_transform_vertex(tri[2], model, mvp, (i + 2));
        mr_draw_triangle(&cmd);
    }

    return MR_OK;
}

mr_result_t mr_model_draw(struct mr_fb_desc* fb, const struct mr_model* model, const mat4f32 cam_vp)
{
    // Compute the MVP matrix
    mat4f32 model_mat, mvp;
    mr_model_matrix(model_mat, model);
    mat4f32_mulmat(mvp, cam_vp, model_mat);

    // Mesh sanity checks
    if (model->mesh.stride % sizeof(f32) != 0) return MR_INVALID_FORMAT;

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
