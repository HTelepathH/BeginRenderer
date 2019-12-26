#ifndef BOBJECT_H 
#define BOBJECT_H

#include "BVertexBuffer.h"

using vertBuffer   =  BVertexBuffer<float3>;
using indexBuffer  =  BVertexBuffer<size_t>;

class BObject
{
public:
    BObject() = default;
    BObject(vertBuffer* vp, indexBuffer* ip) : 
        vb_ptr(vp), ib_ptr(ip) { }
    ~BObject()
    {
        vb_ptr = nullptr;
        ib_ptr = nullptr;
    }

protected:
    vertBuffer* vb_ptr;
    indexBuffer* ib_ptr;
};

class Cube : public BObject
{
public:
    Cube() = default;
    Cube(int);
//    Cube(int, int, int);
    Cube(const Cube&);
//    Cube(Cube&&) noexcept;
    Cube& operator=(const Cube&);
//    Cube& operator=(Cube&&) noexcept;
    ~Cube();

private:
    int a, b, c;
};

#endif