#ifndef BMATRIX_H
#define BMATRIX_H

#include <cmath>
#include "vec3.h"

class BMat
{
    friend std::ostream& operator<<(std::ostream&, const BMat&);
public:
    BMat();
    BMat(size_t);
    BMat(const float3&, const float3&, const float3&, int axis = 0);
    
    BMat(const BMat&);
    BMat(BMat&&) noexcept;
    BMat& operator=(const BMat&);
    BMat& operator=(BMat&&) noexcept;
    ~BMat();

    float& operator[](size_t i);
    float operator[](size_t i) const;

    void setCamMat(float3 pos, float3 look_dir);
    void setVPMat(int w, int h);
    void setPersMat(float fov, int w, int h, float n = 0.1, float f = 100.0);
    void setModelMat(time_t rotate);

    BMat inverse();
    BMat transpose();
    BMat inverse_transpose();
    BMat adjoint();
    float det();

    size_t row = 4;
    size_t col = 4;
    float* mat;
};

inline std::ostream& operator<<(std::ostream& os, const BMat& m)
{
    for(size_t i = 0; i < m.row; ++i)
    {
        for(size_t j = 0; j < m.col; ++j)
            os << m[i * m.row + j] << " ";
        os << std::endl;
    }
    return os;
}

inline BMat operator*(const BMat& a, const BMat& b)
{
    BMat temp(a.row);
    if(a.col != b.row)
    {
        std::cout << "BMat: dimension dismatch, operator '*' " << std::endl;
    }
    else
    {
        for(size_t r = 0; r < a.row; ++r)
        {
            for(size_t c = 0; c < a.row; ++c)
            {
                for(size_t k = 0; k < a.row; ++k)
                {
                    temp[r * a.col + c] += a[r * a.col + k] * b[k * a.col + c];                  
                }
            }
        }
    }
    return temp;    
}

inline float4 operator*(const BMat& a, const float4& b)
{
    float4 temp(0);
    for(size_t r = 0; r < a.row; ++r)
    {
        for(size_t k = 0; k < a.row; ++k)
            temp[r] += a[r * a.col + k] * b[k];                  
    }
    return temp;      
}

inline float3 operator*(const BMat& a, const float3& b)
{
    if(a.row!= 3) return float3(0);
    float3 temp{};
    for(size_t r = 0; r < 3; ++r)
    {
        for(size_t k = 0; k < 3; ++k)
            temp[r] += a[r * a.col + k] * b[k];                  
    }
    return temp;      
}

inline float clamp(float f)
{
    if (f > 1) f = 1.0f;
    if (f < 0) f = 0.0f;
    return f;
}

inline float3 clamp(float3 f)
{
    return float3(clamp(f[0]), clamp(f[1]), clamp(f[2]));
}

#endif
