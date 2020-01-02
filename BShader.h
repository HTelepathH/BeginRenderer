#ifndef BSHADER_H 
#define BSHADER_H

#include "math/vec3.h"
#include "BModel.h"
#include "BCamera.h"

class BShader
{
public:
    virtual float3 vertex_shader(int f_dix, int v_idx) = 0;
    virtual bool fragment_shader(float3 bary_coord, float3& color) = 0;
    virtual ~BShader() = default;
    BModel* model;
    BCamera* cam;
    float* shadow_buffer;
};

static inline BMat calc_TBN(float2* uv, float3* vert, float3& vert_norm)
{
    float3 e2 = vert[2] - vert[0];
    float3 e1 = vert[1] - vert[0];
    float2 d2 = uv[2] - uv[0];
    float2 d1 = uv[1] - uv[0];
    float2 l0(d1[1], -d2[1]) , l1(-d1[0], d2[0]);
    float2 col0(e2[0], e1[0]) , col1(e2[1], e1[1]) , col2(e2[2], e1[2]); 
    float k = 1 / (d2[0] * d1[1] - d2[1] * d1[0]);
    float3 T(l0 * col0, l0 * col1, l0 * col2);
    T *= k;
    float3 B(l1 * col0, l1 * col1, l1 * col2);
    B *= k;
    return BMat(T.normalize(), B.normalize(), vert_norm.normalize(), 1);
}

class BlinnPhongShader_world : public BShader
{
public:
    float2 tex_coord[3]{};
    float3 light_pos = float3(1.0f);
    
    /*world space pos */
    float3 vert_pos[3];
    float3 norm_pos[3];

    virtual float3 vertex_shader(int f_idx, int v_idx) override
    {   
        tex_coord[v_idx] = model->tex(f_idx, v_idx);
        float3 vert_coord = model->vert(f_idx, v_idx);
        /* world pos */
        vert_pos[v_idx] = vert_coord;// * modelMat
        /*ndc pos*/
        float3 gl_vertex = (cam->pers_mat * cam->cam_mat * float4(vert_coord)).homogenous().no_w();
        return gl_vertex;
    }
    virtual bool fragment_shader(float3 bary_coord, float3& color) override
    {   
        //interp
        float2 uv = bary_coord[0] * tex_coord[0] + bary_coord[1] * tex_coord[1] + bary_coord[2] * tex_coord[2];
        float3 pos = bary_coord[0] * vert_pos[0] + bary_coord[1] * vert_pos[1] + bary_coord[2] * vert_pos[2];
        float3 light_dir = (light_pos - pos).normalize();
        //diffuse
        int3 tex_col = model->tex_color(uv);
        float3 tex_color = float3(tex_col[0], tex_col[1], tex_col[2]) / 255.0;
        //ambient
        color = 0.1 * tex_color;
        //norm
        float3 norm = model->tex_norm(uv);
        //specular
        float diffuse = fmax(0, dot(norm, light_dir));
        color = 0.4 * diffuse * tex_color;

        float3 half = (light_dir - cam->look_vec()).normalize();
        float3 tex_spec = model->tex_spec(uv);
        float3 specular{};
        for(int i = 0; i < 3; ++i)
        {
            specular[i] = pow(dot(half, norm), tex_spec[i] + 10) * tex_color[i];
        }
        color += 0.4 * diffuse * tex_color + 3 * specular;

        return false;
    }
};

class BlinnPhongShadowShader_world : public BShader
{
public:
    float2 tex_coord[3]{};
    float3 light_pos = float3(1.0f);
    BMat pers_cam_transform{};

    /*world space pos */
    float3 vert_pos[3];
    float3 norm_pos[3];

    virtual float3 vertex_shader(int f_idx, int v_idx) override
    {   
        tex_coord[v_idx] = model->tex(f_idx, v_idx);
        float3 vert_coord = model->vert(f_idx, v_idx);
        /* world pos */
        vert_pos[v_idx] = vert_coord;// * modelMat
        /*ndc pos*/
        float3 gl_vertex = (cam->pers_mat * cam->cam_mat * float4(vert_coord)).homogenous().no_w();
        return gl_vertex;
    }
    virtual bool fragment_shader(float3 bary_coord, float3& color) override
    {   
        //interp
        float2 uv = bary_coord[0] * tex_coord[0] + bary_coord[1] * tex_coord[1] + bary_coord[2] * tex_coord[2];
        float3 pos = bary_coord[0] * vert_pos[0] + bary_coord[1] * vert_pos[1] + bary_coord[2] * vert_pos[2];
        float3 light_dir = (light_pos - pos).normalize();
        //diffuse
        int3 tex_col = model->tex_color(uv);
        float3 tex_color = float3(tex_col[0], tex_col[1], tex_col[2]) / 255.0;
        //ambient
        color = 0.2 * tex_color;
        //norm
        float3 norm = model->tex_norm(uv);
        //specular
        float diffuse = fmax(0, dot(norm, light_dir));
        color = 0.5 * diffuse * tex_color;

        float3 proj_pos = (pers_cam_transform * float4(pos)).homogenous().no_w();
        if(!is_shadowed(proj_pos))
        { 
            float3 half = (light_dir - cam->look_vec()).normalize();
            float3 tex_spec = model->tex_spec(uv);
            float3 specular{};
            for(int i = 0; i < 3; ++i)
            {
                specular[i] = pow(dot(half, norm), tex_spec[i]) * tex_color[i];
            }
            color += 0.3 * diffuse * tex_color + specular;
        } 
        return false;
    }

private:
    bool is_shadowed(const float3& pos)
    {
        float3 screen_pos = (cam->vp_mat * float4(pos)).no_w(); 
        int col = static_cast<int>(screen_pos.x());
        int row = static_cast<int>(screen_pos.y());
        if(pos.z() + 0.014 < shadow_buffer[cam->width * row + col])
            return true;
        else
            return false;
    }
};

class ShadowShader_world : public BShader
{
public:
    virtual float3 vertex_shader(int f_idx, int v_idx) override
    {   
        float3 vert_coord = model->vert(f_idx, v_idx);
        /*ndc pos*/
        float3 gl_vertex = (cam->pers_mat * cam->cam_mat * float4(vert_coord)).homogenous().no_w();
        return gl_vertex;
    }
    virtual bool fragment_shader(float3 bary_coord, float3& color) override
    {   
        return false;
    }
};

class BlinnPhongShader_tangent : public BShader
{
public:
    float2 tex_coord[3]{}; // uv
    float3 t_norm[3]{}; //world norm
    float3 triangle[3]{}; //world vert
    float3 light_dir = float3(0, 0, 1.0f);
    
    virtual float3 vertex_shader(int f_idx, int v_idx) override
    {
        BMat projection = cam->pers_mat * cam->cam_mat;
        float3 vert_coord = model->vert(f_idx, v_idx);
        tex_coord[v_idx] = model->tex(f_idx, v_idx);

        t_norm[v_idx] = model->norm(f_idx, v_idx);   
        triangle[v_idx] = vert_coord;

        float3 gl_vertex = (cam->pers_mat * cam->cam_mat * float4(vert_coord)).homogenous().no_w();
        return gl_vertex;
    }
    virtual bool fragment_shader(float3 bary_coord, float3& color) override
    {   
        //tex_interp
        float2 uv = bary_coord[0] * tex_coord[0] + bary_coord[1] * tex_coord[1] + bary_coord[2] * tex_coord[2];
        float3 bn = bary_coord[0] * t_norm[0] + bary_coord[1] * t_norm[1] + bary_coord[2] * t_norm[2];
        
        //diffuse texture
        int3 tex_col = model->tex_color(uv);
        float3 tex_color = float3(tex_col[0], tex_col[1], tex_col[2]) / 255.0;
        //norm
        BMat TBN = calc_TBN(tex_coord, triangle, bn);
        
        float3 norm = (TBN * model->tex_norm(uv)).normalize(); 

        float3 T(TBN[0], TBN[3], TBN[6]);
        float3 B(TBN[1], TBN[4], TBN[7]);

/* 
        float3 half = (light_dir - cam->look_vec()).normalize();
        float3 tex_spec = model->tex_spec(uv);
        float3 specular{};
        for(int i = 0; i < 3; ++i)
        {
            specular[i] = pow(dot(half, norm), tex_spec[i]) * tex_color[i];
        } 
*/
        float diffuse = fmax(0, dot(norm, light_dir));
        color = norm;// 0.8 * diffuse * tex_color + 0.7 * specular;
        return false;
    }
};

#endif