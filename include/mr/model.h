#ifndef MR_MODEL_H
#define MR_MODEL_H

#include "mr/fb.h"
#include "mr/types.h"

enum mr_model_type
{
    MR_MODEL_LINES,
    MR_MODEL_TRIANGLES,
};

typedef u8 mr_model_type_t;

struct mr_mesh
{
    // Mesh data
    const f32* vertices;
    const u16* indices;
    u32 num_indices;
    u16 stride; // Stride of vertices in bytes, must be multiple of sizeof(f32) (4 bytes)
    mr_model_type_t type;
};

struct mr_model
{
    // Mesh
    struct mr_mesh mesh;

    // TRS
    v3f32 translation;
    v3f32 rotation;
    v3f32 scale;

    // Shader
    void* shader_func;
    void* shader_data;
};

void mr_model_matrix(mat4f32 matrix, const struct mr_model* model);
mr_result_t mr_model_draw(
    struct mr_fb_desc* fb, const struct mr_model* model, const mat4f32 cam_vp);

#endif