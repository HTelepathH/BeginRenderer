#include "BMatrix.h"

//----------------------------------------------------------------------
// Bmat operations
//----------------------------------------------------------------------
BMat::BMat()
{
    mat = new float[row * col];
    for(size_t i = 0; i < row * col; ++i)
        mat[i] = 0;
}

BMat::BMat(size_t sz):
    row(sz), col(sz)
{
    mat = new float[row * col];
    for(size_t i = 0; i < row * col; ++i)
        mat[i] = 0;
}

BMat::BMat(const float3& i, const float3& j, const float3& k, int axis)
{
    row = col = 3;
    mat = new float[row * col];
    if(axis) //axis == 1 : col
    {
        mat[0] = i.x(); mat[1] = j.x(); mat[2] = k.x();
        mat[3] = i.y(); mat[4] = j.y(); mat[5] = k.y();
        mat[6] = i.z(); mat[7] = j.z(); mat[8] = k.z();
    }
    else //axis == 0 : row
    {
        mat[0] = i.x(); mat[1] = i.y(); mat[2] = i.z();
        mat[3] = j.x(); mat[4] = j.y(); mat[5] = j.z();
        mat[6] = k.x(); mat[7] = k.y(); mat[8] = k.z();
    }
    
}

BMat::BMat(const BMat& rhs)
{
    row = rhs.row;
    col = rhs.col;
    mat = new float[row * col]; 
    for(size_t i = 0; i < row * col; ++i)
        mat[i] *= rhs.mat[i];
}
BMat::BMat(BMat&& rhs) noexcept
{
    row = rhs.row;
    col = rhs.col;
    mat = rhs.mat;
    rhs.mat = nullptr;
}
BMat& BMat::operator=(const BMat& rhs)
{
    float* temp = new float[row * col]; 
    for(size_t i = 0; i < row * col; ++i)
        temp[i] *= rhs.mat[i];
    row = rhs.row;
    col = rhs.col;
    delete [] mat;
    mat = temp;
    return *this;
}
BMat& BMat::operator=(BMat&& rhs) noexcept
{
    row = rhs.row;
    col = rhs.col;
    mat = rhs.mat;
    rhs.mat = nullptr;
    return *this;
}

BMat::~BMat()
{
    delete [] mat;
}

float& BMat::operator[](size_t i)
{
    return *(mat + i);
}

float BMat::operator[](size_t i) const
{
    return *(mat + i);
}

//----------------------------------------------------------------------
// advance operations
//----------------------------------------------------------------------
static float minor(const BMat& m, int r, int c) {
    BMat cut_down(m.row - 1);
    int rc = cut_down.row, cc = cut_down.col;
    int i, j;
    for (i = 0; i < rc; i++) {
        for (j = 0; j < cc; j++) {
            int row = i < r ? i : i + 1;
            int col = j < c ? j : j + 1;
            cut_down[i * cc + j] = m[row * m.col + col];
        }
    }
    return cut_down.det();
}

static float cofactor(const BMat& m, int r, int c) {
    float sign = (r + c) % 2 == 0 ? 1.0f : -1.0f;
    float minr = minor(m, r, c);
    return sign * minr;
}

BMat BMat::inverse()
{
    return inverse_transpose().transpose();
}

BMat BMat::transpose()
{
    BMat transpose(row);
    int i, j;
    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            transpose[i * col + j] = mat[j * row + i];
        }
    }
    return transpose;
}

BMat BMat::inverse_transpose()
{
    BMat adjoint = this->adjoint();
    BMat inverse_transpose(row);
    float determinant = 0, inv_determinant;

    for (int i = 0; i < col; i++) {
        determinant += mat[i] * adjoint[i];
    }
    inv_determinant = 1 / determinant;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            inverse_transpose[i * col + j] = adjoint[i * col + j] * inv_determinant;
        }
    }
    return inverse_transpose;
}

BMat BMat::adjoint()
{
    BMat adjoint(row);
    int i, j;
    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            adjoint[i * col + j] = cofactor(*this, i, j);
        }
    }
    return adjoint;
}

float BMat::det()
{
    if(col == 3 && row == 3)
    {
        float a = +mat[0] * (mat[4] * mat[8] - mat[5] * mat[7]);
        float b = -mat[1] * (mat[3] * mat[8] - mat[5] * mat[6]);
        float c = +mat[2] * (mat[3] * mat[7] - mat[4] * mat[6]);
        return a + b + c;
    }
    else if(col == 2 && row == 2)
    {
        return mat[0] * mat[3] - mat[1] * mat[2];
    }
    else
    {
        float result = 0;
        for(int i = 0 ; i < 4 ; ++i)
        {
           result += mat[i] * minor(*this, 0, i); 
        }
        return result;
    }    
}

//----------------------------------------------------------------------
// CamMat
//----------------------------------------------------------------------
void BMat::setCamMat(float3 pos, float3 look_dir)
{
    float3 w = (-look_dir).normalize();
    float3 u = cross(float3(0.0, 1.0, 0.0), w).normalize();
    float3 v = cross(w, u).normalize();
    mat[0] = u.x(); mat[1] = u.y(); mat[2] = u.z(); mat[3] = dot(u, -pos);
    mat[4] = v.x(); mat[5] = v.y(); mat[6] = v.z(); mat[7] = dot(v, -pos);
    mat[8] = w.x(); mat[9] = w.y(); mat[10] = w.z(); mat[11] = dot(w, -pos);
    mat[15] = 1.0;    
}

//----------------------------------------------------------------------
// VPMat
//----------------------------------------------------------------------
void BMat::setVPMat(int w, int h)
{
    mat[0] = w * 0.5; mat[3] = (w - 1) * 0.5;
    mat[5] = w * 0.5; mat[7] = (h - 1) * 0.5;
    mat[10] = 1.0; mat[15] = 1.0;
}

//----------------------------------------------------------------------
// PersMat
//----------------------------------------------------------------------
void BMat::setPersMat(float fov, int w, int h, float n, float f)
{
    n = 1; f = 10;
    float r = 1.0f * w / h;
    float tan_2 = tan(fov * 3.1415926 / 360);
    mat[0] = 1 / (r * tan_2);
    mat[5] = 1 / tan_2;
    mat[10] = f / (f - n);
    mat[11] = 1.0f;
    mat[14] = -n * f / (f - n);
}

//----------------------------------------------------------------------
// ModelMat
//----------------------------------------------------------------------
void BMat::setModelMat(time_t rotate)
{
    mat[0] = cos(rotate); mat[2] = sin(rotate);
    mat[5] = 1;
    mat[8] = 0-sin(rotate); mat[10] = cos(rotate);
    mat[15] = 1;
}