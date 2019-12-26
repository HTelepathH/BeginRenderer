#include "BObject.h"

Cube::Cube(int e)
{
    a = b = c = e;
    vb_ptr = new vertBuffer({
        float3(  e / 2, -e / 2,  e / 2),
        float3(  e / 2,  e / 2,  e / 2),
        float3( -e / 2,  e / 2,  e / 2),
        float3( -e / 2, -e / 2,  e / 2),
        float3(  e / 2, -e / 2, -e / 2),
        float3(  e / 2,  e / 2, -e / 2),
        float3( -e / 2,  e / 2, -e / 2),
        float3( -e / 2, -e / 2, -e / 2)
    });

    ib_ptr = new indexBuffer({
        0, 1, 3, 3, 1, 2,
        0, 4, 5, 5, 1, 0,
        4, 7, 6, 6, 5, 4,
        7, 3, 2, 2, 6, 7,
        1, 5, 6, 6, 2, 1,
        0, 4, 7, 7, 3, 0
    });
}

Cube::Cube(const Cube& rhs):
    a(rhs.a), b(rhs.b), c(rhs.c)
{
    vb_ptr = new vertBuffer(*rhs.vb_ptr);
    ib_ptr = new indexBuffer(*rhs.ib_ptr);
}

Cube& Cube::operator=(const Cube& rhs)
{
    vertBuffer* temp_vb_ptr = new vertBuffer(*rhs.vb_ptr);
    indexBuffer* temp_ib_ptr = new indexBuffer(*rhs.ib_ptr);
    delete vb_ptr;
    delete ib_ptr;
    vb_ptr = temp_vb_ptr;
    ib_ptr = temp_ib_ptr;
    return *this;
}

Cube::~Cube()
{
    delete vb_ptr;
    delete ib_ptr;
}