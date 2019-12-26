#ifndef BCAMERA_H 
#define BCAMERA_H

#include "math/BMatrix.h"
#include <ctime>

class BCamera
{
public:
    BCamera() = default;
    BCamera(int w, int h, float fov = 45.0, 
            float3 pos = float3(0.0, 0.0, 2.5), 
            float3 look_dir = float3(0.0, 0.0, -1.0));
    void update(int2, short);
    float3 look_vec() const { return look_dir; }

    ~BCamera();
    float3 to_screen(float3);
    BMat cam_mat;
    BMat vp_mat;
    BMat pers_mat;
    
    float3 pos;
    float3 look_dir;
    int width, height;
    float theta, phi;
    float fov;
};

#endif