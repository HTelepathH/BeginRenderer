#include <iostream>
#include <cstring>
#include <iomanip>

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

int main()
{
    BMat<4> m;
    BMat<4> cm(m);
    m[2][1] = 6.7;
    float c  = m[2][2];
    std::cout << m << std::endl;
    std::cout << m[2][1] << " " << c << std::endl;
    std::cout << "adjoint" << std::endl;
    std::cout << m.adjoint() << std::endl;
    std::cout << "inverse" << std::endl;
    std::cout << m.inverse() << std::endl;
    std::cout << "inverse_transpose" << std::endl;
    std::cout << m.inverse_transpose() << std::endl;
    std::cout << "transpose" << std::endl;
    std::cout << m.transpose() << std::endl;

    
}

