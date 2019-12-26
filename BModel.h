#ifndef BMODEL_H 
#define BMODEL_H

#include "math/vec3.h"
#include "BTexture.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

struct index
{
    int vert_id[3];
    int tex_id[3];
    int norm_id[3];
};

class BModel
{
public:
    BModel() = default;
    BModel(const std::string&);
    size_t nverts() const { return vert_vec.size();}
    size_t nfaces() const { return face_idx_vec.size();}
    float3 vert(size_t i, size_t j);
    float2 tex(size_t i, size_t j);
    float3 norm(size_t i, size_t j);
    bool load_texture(const std::string&, int);
    int3 tex_color(float2);
    float3 tex_norm(float2);
    float3 tex_spec(float2);
    void debug(const char*);
    ~BModel();
    
    void debug()
    {
        std::cout << "vert_vec : " << nverts() << std::endl;
        std::cout << "tex_vec : " << tex_vec.size() << std::endl;
        std::cout << "norm_vec : " << norm_vec.size() << std::endl;
        std::cout << "face_idx_vec : " << nfaces() << std::endl;
    }

protected:
    BTexture* diffuse_ptr;
    BTexture* norm_ptr;
    BTexture* spec_ptr;
    std::vector<float3> vert_vec;
    std::vector<float2> tex_vec;
    std::vector<float3> norm_vec;
    std::vector<index> face_idx_vec;
    
};

#endif