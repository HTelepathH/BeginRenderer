#include "BTexture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

BTexture::BTexture(const char* pic_name)
{
    tex_ptr = stbi_load(pic_name, &w, &h, &n, 0);
}

int3 BTexture::at(float2 uv)
{   
    //clamp(uv);
    int x = static_cast<int>(w * uv.x);
    int y = static_cast<int>(h * (1.0f - uv.y));
    unsigned char* color = tex_ptr + w * y * n + x * n;
    return int3(color[0], color[1], color[2]);
}

void BTexture::clamp(float2& uv)
{
    uv.x = fmax(0, uv.x);
    uv.x = fmin(1, uv.x);
    uv.y = fmax(0, uv.y);
    uv.y = fmin(1, uv.y);
}

BTexture::~BTexture()
{
    stbi_image_free(tex_ptr);
}