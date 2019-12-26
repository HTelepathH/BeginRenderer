#ifndef BDEVICE_H
#define BDEVICE_H

#include "BWindow.h"
#include "BModel.h"
#include "BCamera.h"
#include "math/vec3.h"
#include <limits>
#include <ctime>
#include "BShader.h"

class BDevice {
public:
    BDevice(size_t w, size_t h);
    ~BDevice();

    void main_loop();
    size_t width() const { return mwidth; }
    size_t height() const { return mheight;}

    void TEST_show_model_wireframe();
    void TEST_show_model_shader();
    void TEST_show_shadow_shader();

private:
    BWindow window;
    size_t mwidth, mheight;

    //window ops;
    HDC hardware_dc;
    HDC memory_dc;
    HBITMAP hBackbuffer;
    MSG msg;
    bool window_close = false;
    unsigned char* real_buffer; // allocated by memory_dc
    float* z_buffer; // new delete
    float* shadow_buffer;

    void initialize_buffers();
    void destroy_buffers();
    void copy_and_poll_event();
    void refresh_z();
    void refresh_real_buffer();

    void draw_line(int2, int2);
    void draw_point(size_t x, size_t y, float3 color);
    void draw_point(size_t x, size_t y, int3 color);
    void draw_triangle_shader(float3, float3, float3, BShader&);
    void draw_triangle_shadow(float3, float3, float3, BShader&);

};

#endif