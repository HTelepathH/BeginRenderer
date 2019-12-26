#include "BModel.h"

BModel::BModel(const std::string& filename)
{
    std::ifstream infile;
    infile.open(filename.c_str(), std::ifstream::in);
    if(infile.fail())
    {
        std::cout << "load model '" << filename << "' failed !" << std::endl;
        return;
    }
    std::string line;
    char trash;
    while(!infile.eof())
    {
        std::getline(infile, line);
        std::istringstream iss(line.c_str());
        if(!line.compare(0, 2, "v "))
        {
            iss >> trash;
            float3 v;
            for(int i = 0; i < 3 ; ++i)
                iss >> v[i];
            vert_vec.push_back(v);
            //std::cout << "v " << v[0]<< std::endl;
        }
        else if(!line.compare(0, 2, "vt"))
        {
            iss >> trash;
            iss >> trash;
            float2 v;
            iss >> v.x;
            iss >> v.y;
            tex_vec.push_back(v); 
            //std::cout << "vt" << v.x << " " << v.y<< std::endl;  
        }
        else if(!line.compare(0, 2, "vn"))
        {
            iss >> trash;
            iss >> trash;
            float3 v;
            for(int i = 0; i < 3 ; ++i)
                iss >> v[i];
            norm_vec.push_back(v);   
            //std::cout << "vn " << v[0]<< std::endl;
        }
        else if(!line.compare(0, 2, "f "))
        {
            iss >> trash;
            index id;
            for(int i = 0; i < 3 ; ++i)
            {
                iss >> id.vert_id[i] >> trash >> id.tex_id[i] >> trash >> id.norm_id[i];
                --id.vert_id[i];
                --id.tex_id[i];
                --id.norm_id[i];
            }
            face_idx_vec.push_back(id);  
            //std::cout << "f " << id.vert_id[0] << " " << id.vert_id[1] << " " << id.vert_id[2] << std::endl;
        }
        else
        {
            continue;
        }
        
    }
    infile.close();
}

BModel::~BModel()
{
    delete diffuse_ptr;
    delete norm_ptr;
}

bool BModel::load_texture(const std::string& pic_name, int type)
{
    // 0 diffuse
    // 1 norm
    // 2 spec
    if(type == 0)
    {
        diffuse_ptr = new BTexture(pic_name.c_str());
        if(diffuse_ptr == nullptr) 
            return false;
        else 
            return true;
    }
    else if (type == 1)
    {
        norm_ptr = new BTexture(pic_name.c_str());
        if(norm_ptr == nullptr) 
            return false;
        else 
            return true;
    }  
    else
    {
        spec_ptr = new BTexture(pic_name.c_str());
        if(spec_ptr == nullptr) 
            return false;
        else 
            return true;       
    } 
}

void BModel::debug(const char* file_name)
{
    std::ofstream ofile;
    ofile.open(file_name);
    ofile << "vert_vec" << std::endl;
    for(auto& v : vert_vec)
    {
        ofile << v;
    }
    ofile.close();
}

//----------------------------------------------------------------------
// tex color
//----------------------------------------------------------------------
int3 BModel::tex_color(float2 uv)
{
    return diffuse_ptr->at(uv);
}

float3 BModel::tex_norm(float2 uv)
{
    int3 int_norm = norm_ptr->at(uv);
    return float3(int_norm[0], int_norm[1], int_norm[2]).normalize();
}

float3 BModel::tex_spec(float2 uv)
{
    int3 int_spec = spec_ptr->at(uv);
    return float3(int_spec[0], int_spec[1], int_spec[2]);
}

//----------------------------------------------------------------------
// indexing
//----------------------------------------------------------------------
float3 BModel::vert(size_t i, size_t j)
{
    index fidx = face_idx_vec[i];
    return vert_vec[fidx.vert_id[j]];
}

float2 BModel::tex(size_t i, size_t j)
{
    index fidx = face_idx_vec[i];
    return tex_vec[fidx.tex_id[j]];
}

float3 BModel::norm(size_t i, size_t j)
{
    index fidx = face_idx_vec[i];
    return norm_vec[fidx.norm_id[j]];
}