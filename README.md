# BeginRenderer
a real-time soft renderer, following [tinyrenderer tutorials](https://github.com/ssloy/tinyrenderer/wiki).

## features
- orbital camera with zoom
- uv mapping
- phong shading
- hard shadows
- using GDI for window

## compile
under clang++:

clang++ main.cpp -O3 BWindow.cpp BDevice.cpp BModel.cpp BTexture.cpp math/BMatrix.cpp BCamera.cpp

## third party
using [nothins/stb](https://github.com/nothings/stb) for image decoding
