#include "BCamera.h"

BCamera::BCamera(int w, int h, float fov, float3 _pos, float3 _look_dir) :
    pos(_pos), look_dir(_look_dir), width(w), height(h), fov(fov)
{
    cam_mat.setCamMat(pos, look_dir);
    vp_mat.setVPMat(w, h);
    pers_mat.setPersMat(fov, w, h);
}

float3 BCamera::to_screen(float3 world_coord)
{
    return (vp_mat * ((pers_mat * cam_mat * float4(world_coord)).homogenous())).no_w();
}

void BCamera::update(int2 offset, short scroll)
{
    // orbital move
    float factor = 1.5 * 3.1415926;
    float radius = pos.length();
    theta = atan2(pos.x(), pos.z());
    phi = acos(pos.y() / radius);
    theta -= factor * offset.x / width;
    phi -= factor * offset.y / height;
    if(phi > 3.1415) phi =  3.1415;
    if(phi < 1e-5) phi = 1e-5;

    //std::cout << theta << " " <<  phi << std::endl;

    pos = float3(radius * sin(phi) * sin(theta), 
            radius * cos(phi),
            radius * sin(phi) * cos(theta));
    look_dir = float3(0) - pos;

    cam_mat.setCamMat(pos, look_dir);

    //scroll fov
    float delta_fov = scroll * 0.01;
    fov -= delta_fov;
    if(fov > 160.0) fov = 160.0;
    if(fov < 38.0) fov = 38.0;
    pers_mat.setPersMat(fov, width, height);
}

BCamera::~BCamera()
{

}
