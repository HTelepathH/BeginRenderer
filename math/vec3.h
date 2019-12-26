#ifndef VEC_H
#define VEC_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <stdlib.h>
#include <iostream>

//----------------------------------------------------------------------
// vec3
//----------------------------------------------------------------------
template<typename T>
class vec3 
{
public:
    vec3() = default;
    vec3(T all) { e[0] = e[1] = e[2] = all;}
    vec3(T e0, T e1, T e2) { e[0] = e0; e[1] = e1; e[2] = e2;}
    T x() const { return e[0]; }
    T y() const { return e[1]; }
    T z() const { return e[2]; }
    T r() const { return e[0]; }
    T g() const { return e[1]; }
    T b() const { return e[2]; }

    const vec3& operator+() const { return *this;}
    vec3 operator-() const { return vec3(0-e[0], 0-e[1], 0-e[2]);}
    T operator[](int i) const { return e[i];}
    T& operator[](int i) {return e[i];}

    vec3& operator+=(const vec3& v2);
    vec3& operator-=(const vec3& v2);
    vec3& operator*=(const vec3& v2);
    vec3& operator/=(const vec3& v2);
    vec3& operator*=(const float t);
    vec3& operator/=(const float t);

    T length() const { return sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]); }
    T squared_length() const { return e[0]*e[0] + e[1]*e[1] + e[2]*e[2]; }
    vec3& normalize();

    T e[3];
};

using float3 =  vec3<float>;
using int3   =  vec3<int>;

//----------------------------------------------------------------------
// vec2
//----------------------------------------------------------------------
template<typename T>
class vec2
{
public:
    vec2() = default;
    vec2(T _x, T _y) :
        x(_x), y(_y){ }
    T x, y;
    T operator[](int i) const { return i == 0 ? x : y;}
    T& operator[](int i) {return i == 0 ? x : y;}
};

template<typename T>
inline void swap(vec2<T>& lhs, vec2<T>& rhs)
{
    using std::swap;    //find in parameters, if not use std::
    swap(lhs.x, rhs.x);
    swap(lhs.y, rhs.y);
}

template<typename T>
vec2<T> operator*(const vec2<T>& v, float f)
{
    return vec2<T>(v.x * f, v.y * f);
}

template<typename T>
vec2<T> operator*(float f, const vec2<T>& v)
{
    return vec2<T>(v.x * f, v.y * f);
}

template<typename T>
vec2<T> operator+(const vec2<T>& v1, const vec2<T>& v2)
{
    return vec2<T>(v1.x + v2.x, v1.y + v2.y);
}

template<typename T>
vec2<T> operator-(const vec2<T>& v1, const vec2<T>& v2)
{
    return vec2<T>(v1.x - v2.x, v1.y - v2.y);
}

template<typename T>
float operator*(const vec2<T>& v1, const vec2<T>& v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

template<typename T>
vec2<T> operator/(const vec2<T>& v, float f)
{
    return vec2<T>(v.x / f, v.y / f);
}

template<typename T>
bool operator==(const vec2<T>& v1, const vec2<T>& v2)
{
    return v1.x == v2.x && v1.y == v2.y;
}

template<typename T>
bool operator!=(const vec2<T>& v1, const vec2<T>& v2)
{
    return !(v1 == v2);
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const vec2<T>& v)
{
    os << "(" << v.x << ", " << v.y << ")" << std::endl;
    return os;
}

using int2   =  vec2<int>;
using float2 =  vec2<float>;


//----------------------------------------------------------------------
// float4
//----------------------------------------------------------------------
class float4
{
public:
    float4() = default;
    float4(float all)
    {
        e[0] = e[1] = e[2] = all; e[3] = 1;
    }
    float4(float3 v3)
    {
        e[0] = v3[0]; e[1] = v3[1]; e[2] = v3[2]; e[3] = 1;
    }
    float4(float e0, float e1, float e2, float e3)
    {
        e[0] = e0; e[1] = e1; e[2] = e2; e[3] = e3;
    }

    float operator[](int i) const { return e[i];}
    float& operator[](int i) { return e[i];}
    float3 no_w()
    {
        return float3(e[0], e[1], e[2]);
    }
    float4& homogenous()
    {
        e[0] /= e[3]; e[1] /= e[3]; e[2] /= e[3]; e[3] /= e[3];
        return *this;
    }
    float x() const { return e[0]; }
    float y() const { return e[1]; }
    float z() const { return e[2]; }
    float w() const { return e[3]; }

    float r() const { return e[0]; }
    float g() const { return e[1]; }
    float b() const { return e[2]; }
    float a() const { return e[3]; }

    float e[4];
};

class temp4
{
public:
    temp4() = default;
    temp4(int _x, int _y, float _z, float3 c) :
        x(_x), y(_y), z(_z), color(c) { }
    int x, y;
    float z;
    float3 color;
};

inline std::ostream& operator<<(std::ostream& os, const float4& t)
{
    os <<"(" << t.x() <<", "<< t.y() << ", " << t.z()<<", " << t.w() << ")" << std::endl;
    return os;
}
//----------------------------------------------------------------------
// utilities for vec3
//----------------------------------------------------------------------
template <typename T>
inline std::istream& operator>>(std::istream& is, vec3<T>& t)
{
    is >> t.e[0] >> t.e[1] >> t.e[2];
    return is;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const vec3<T>& t)
{
    os <<"(" << t.e[0] <<", "<< t.e[1] << ", " << t.e[2]<< ")" << std::endl;
    return os;
}

template <typename T>
inline vec3<T>& vec3<T>::normalize()
{
    float k = 1.0 / sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]);
    e[0] *= k; e[1] *= k; e[2] *= k;
    return *this;
}

template <typename T>
inline vec3<T> operator+(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3<T>(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}
template <typename T>
inline vec3<T> operator-(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3<T>(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

template <typename T>
inline vec3<T> operator*(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3<T>(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

template <typename T>
inline vec3<T> operator/(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3<T>(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

template <typename T>
inline vec3<T> operator*(float t, const vec3<T>& v)
{
    return vec3<T>(v.e[0] * t, v.e[1] * t, v.e[2] * t);
}

template <typename T>
inline vec3<T> operator/(vec3<T> v, float t)
{
    return vec3<T>(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

template <typename T>
inline vec3<T> operator*(const vec3<T>& v, float t)
{
    return vec3<T>(v.e[0] * t, v.e[1] * t, v.e[2] * t);
}

template <typename T>
inline float dot(const vec3<T>& v1, const vec3<T>& v2)
{
    return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

template <typename T>
inline vec3<T> cross(const vec3<T>& v1, const vec3<T>& v2)
{
    return vec3<T>( v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1],
                0-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0]),
                v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]);
}

template <typename T>
inline vec3<T>& vec3<T>::operator+=(const vec3<T>& v)
{
    e[0] += v.e[0]; e[1] += v.e[1]; e[2] += v.e[2];
    return *this;
}

template <typename T>
inline vec3<T>& vec3<T>::operator*=(const vec3<T>& v)
{
    e[0] *= v.e[0]; e[1] *= v.e[1]; e[2] *= v.e[2];
    return *this;
}

template <typename T>
inline vec3<T>& vec3<T>::operator/=(const vec3<T>& v)
{
    e[0] /= v.e[0]; e[1] /= v.e[1]; e[2] /= v.e[2];
    return *this;
}

template <typename T>
inline vec3<T>& vec3<T>::operator-=(const vec3<T>& v)
{
    e[0] -= v.e[0]; e[1] -= v.e[1]; e[2] -= v.e[2];
    return *this;
}

template <typename T>
inline vec3<T>& vec3<T>::operator*=(const float t)
{
    e[0] *= t; e[1] *= t; e[2] *= t;
    return *this;
}

template <typename T>
inline vec3<T>& vec3<T>::operator/=(const float t)
{
    float k = 1.0 / t;
    e[0] *= k; e[1] *= k; e[2] *= k;
    return *this;
}

template <typename T>
inline vec3<T> unit_vector(vec3<T> v)
{
    return v / v.length();
}

//----------------------------------------------------------------------
// utilities for float4
//----------------------------------------------------------------------
inline float4 operator*(const float4& a, float f)
{
    return float4(a[0] * f, a[1] * f, a[2] * f, a[3] * f);
}
inline float4 operator*(float f, const float4& a)
{
    return float4(a[0] * f, a[1] * f, a[2] * f, a[3] * f);
}
#endif