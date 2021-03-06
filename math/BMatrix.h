#ifndef BMATRIX_H
#define BMATRIX_H

#include <cmath>
#include "vec3.h"

template<uint8_t N>
class BMat
{
public:
    BMat()
    {
        memset(mat, 0, (N * N) * sizeof(float));
        for(int i=0;i < N; ++i)
            mat[i * N + i] = 1.0f;
    }
/*   
    BMat(const BMat& rhs);
    BMat(BMat&&) noexcept;
    BMat& operator=(const BMat&);
    BMat& operator=(BMat&&) noexcept;
    ~BMat() = default;
*/
    float* operator[](uint8_t i)
    {
        return mat + i * row;
    }
    const float* operator[](uint8_t i) const
    {
        return mat + i * row;
    }

    BMat inverse()
    {
        return inverse_transpose().transpose();
    }

    BMat transpose()
    {
        BMat transpose;
        for (int i = 0; i < N; i++) 
            for (int j = 0; j < N; j++) 
                transpose[i][j] = mat[j * N + i];

        return transpose;
    }

    BMat inverse_transpose()
    {
        BMat adjoint = this->adjoint();
        BMat inv_transpose;
        float det = 0, inv_det;

        for (int i = 0; i < col; i++) 
            det += mat[i] * adjoint[i][0];

        inv_det = 1 / det;
        for (int i = 0; i < row; i++) 
            for (int j = 0; j < col; j++) 
                inv_transpose[i][j] = adjoint[i][j] * inv_det;

        return inv_transpose;
    }
    
    BMat adjoint()
    {
        if(N == 1) return *this;
        BMat<N> adjoint;
        for (int r = 0; r < N; r++) 
        {
            for (int c = 0; c < N; c++) 
            {
                float sign = (r + c) % 2 == 0 ? 1.0f : -1.0f;
                float minr = minor(r, c);
                adjoint[r][c] = sign * minr;
            }
        }
        return adjoint;
    }
    float minor(int r, int c)
    {
        BMat<N-1>cut_down;
        for (int i = 0; i < N-1; i++) {
            for (int j = 0; j < N-1; j++) {
                int row = i < r ? i : i + 1;
                int col = j < c ? j : j + 1;
                cut_down[i][j] = mat[row * N + col];
            }
        }
        return cut_down.det();
    }
    float det()
    {
        if(col == 3)
        {
            float a = +mat[0] * (mat[4] * mat[8] - mat[5] * mat[7]);
            float b = -mat[1] * (mat[3] * mat[8] - mat[5] * mat[6]);
            float c = +mat[2] * (mat[3] * mat[7] - mat[4] * mat[6]);
            return a + b + c;   
        }
        float det = 0.0f;
        for(int i = 0; i < N; ++i)
        {
            float sign = i % 2 == 0 ? 1.0f : -1.0f;
            return sign * minor(0, i);
        }
    }

    const uint8_t row = N;
    const uint8_t col = N;
    float mat[N * N];
};

using BMat4 = BMat<4>;
using BMat3 = BMat<3>;

class CamMat : public BMat
{
public:
    CamMat(float3 pos, float3 look_dir);
};

class VPMat : public BMat
{
public:
    VPMat(int w, int h);

};

    void setVPMat(int w, int h);
    void setPersMat(float fov, int w, int h, float n = 0.1, float f = 100.0);
    void setModelMat(time_t rotate);

class PersMat : public BMat
{
public:
    PersMat(float fov, int w, int h, float n = 0.1, float f = 100.0);
};

class ModelMat
{
    ModelMat(time_t rotate, float scale = 1.0f);
};

template<uint8_t N>
std::ostream& operator<<(std::ostream& os, const BMat<N>& m)
{
    os << std::left;
    for(size_t i = 0; i < m.row; ++i)
    {
        for(size_t j = 0; j < m.col; ++j)
            os << std::setw(8) << m[i][j];
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
    if (f > 1.0f) f = 1.0f;
    if (f < 0.0f) f = 0.0f;
    return f;
}

inline float3 clamp(float3 f)
{
    return float3(clamp(f[0]), clamp(f[1]), clamp(f[2]));
}

#endif
