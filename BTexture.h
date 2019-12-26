#ifndef BTEXTURE_H 
#define BTEXTURE_H

#include "math/vec3.h"

class BTexture
{
public:
    BTexture() = default;
    BTexture(const char*);
    //BTexture(const BTexture&);
    //BTexture(BTexture&&) noexcept;
    //BTexture& operator=(const BTexture&);
    //BTexture& operator=(BTexture&&) noexcept;
    int3 at(float2);
    ~BTexture();

private:
    void clamp(float2&);
    unsigned char* tex_ptr;
    int w, h, n;
};

#endif