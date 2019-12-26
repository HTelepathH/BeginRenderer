#include "BDevice.h"

//----------------------------------------------------------------------
// constructor and deconstructor
//----------------------------------------------------------------------
BDevice::BDevice(size_t w, size_t h) :
        mwidth(w), mheight(h)
{
    window = BWindow(w, h);
    initialize_buffers();
}

BDevice::~BDevice()
{

}

//----------------------------------------------------------------------
// main loop
//----------------------------------------------------------------------
void BDevice::main_loop()
{
    window.show();

    MSG msg;

/*
    bool exit = false;
    while(!exit)
    {
        TEST_paint_GetDC();
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
                { exit = true; break; }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
*/
    while(!window_close)
    {
        copy_and_poll_event();
    }

    destroy_buffers();
}

//----------------------------------------------------------------------
// system action
//----------------------------------------------------------------------
void BDevice::initialize_buffers()
{
    hardware_dc = GetDC(window.handle);
    // 创建缓存DC (当前窗口DC的兼容DC)
    memory_dc = CreateCompatibleDC(hardware_dc);
    
    BITMAPINFOHEADER bi_header;
    bi_header.biSize = sizeof(BITMAPINFOHEADER);
    bi_header.biWidth = mwidth;
    bi_header.biHeight = mheight;  /* top-down */
    bi_header.biPlanes = 1;
    bi_header.biBitCount = 32;
    bi_header.biCompression = BI_RGB;

    HBITMAP hBackbuffer;
    //get real_buffer pointer
    hBackbuffer = CreateDIBSection(memory_dc, (BITMAPINFO*)&bi_header,
                                  DIB_RGB_COLORS, (void**)&real_buffer, NULL, 0);

    SelectObject(memory_dc, hBackbuffer);

    SetProp(window.handle, TEXT("ENTRY"), &window);
    window.show();

    z_buffer = new float[mwidth * mheight];
    shadow_buffer = new float[mwidth * mheight];
    refresh_z();
}

void BDevice::destroy_buffers()
{
    RemoveProp(window.handle, TEXT("ENTRY"));
    ReleaseDC(window.handle, hardware_dc);
    DeleteObject(hBackbuffer);
    DeleteDC(memory_dc);
    delete [] z_buffer;
    delete [] shadow_buffer;
}

void BDevice::refresh_z()
{
    for(size_t i = 0; i < mwidth * mheight; ++i)
    {
        z_buffer[i] = -(std::numeric_limits<float>::max)();
        shadow_buffer[i] = -(std::numeric_limits<float>::max)();
    }
}

void BDevice::refresh_real_buffer()
{
    for(size_t i = 0; i < mwidth * mheight * 4; ++i)
        real_buffer[i] = 0;   
}
void BDevice::copy_and_poll_event()
{
    BitBlt(hardware_dc, 0, 0, mwidth, mheight, memory_dc, 0, 0, SRCCOPY);

    if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if(msg.message == WM_QUIT)
            window_close = true;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    else
    {
        //Sleep(500);
    }
}

//----------------------------------------------------------------------
// draw point
//----------------------------------------------------------------------
void BDevice::draw_point(size_t x, size_t y, float3 color)
{
    color = clamp(color);
    unsigned char* p = real_buffer + (x + y * mwidth) * 4;
    p[0] = static_cast<unsigned char>(color.b() * 255);
    p[1] = static_cast<unsigned char>(color.g() * 255);
    p[2] = static_cast<unsigned char>(color.r() * 255);
}
void BDevice::draw_point(size_t x, size_t y, int3 color)
{
    unsigned char* p = real_buffer + (x + y * mwidth) * 4;
    p[0] = static_cast<unsigned char>(color.b());
    p[1] = static_cast<unsigned char>(color.g());
    p[2] = static_cast<unsigned char>(color.r());   
}

//----------------------------------------------------------------------
// draw line
//----------------------------------------------------------------------
void BDevice::draw_line(int2 a, int2 b)
{
    bool steep = false;
    if(std::abs(a.x - b.x) < std::abs(a.y - b.y))
    {
        std::swap(a.x, a.y);
        std::swap(b.x, b.y);
        steep = true;
    }
    if(a.x > b.x)
        swap(a, b);
    int dx = b.x - a.x;
    int dy = b.y - a.y;
    float derror2 =  std::abs(dy) * 2;
    float error2 = 0;
    int y = a.y;
    for(int x = a.x; x < b.x; ++x)
    {
        if(steep)
        {
            draw_point(y, x, float3(1.0));
        } 
        else
        {
            draw_point(x, y, float3(1.0));
        }
        error2 += derror2;
        if(error2 > 0.5)
        {
            y += b.y > a.y ? 1 : -1;
            error2 -= dx * 2;
        }
    }
}

//----------------------------------------------------------------------
// draw triangle
//----------------------------------------------------------------------
template<typename T1>
inline static float f_line(const T1& a, const T1& b, int x, int y)
{
    return static_cast<float>((a.y - b.y) * x + (b.x - a.x) * y + a.x * b.y - b.x * a.y);
}

template<typename T>
inline static float3 calc_bary(const T& a, const T& b, const T& c, int x, int y)
{
    float2 p(x, y);
    float3 s[2];
    for (int i=2; i--; ) {
        s[i][0] = c[i]-a[i];
        s[i][1] = b[i]-a[i];
        s[i][2] = a[i]-p[i];
    }
    float3 u = cross(s[0], s[1]);
    // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
    if (std::abs(u[2]) > 1e-2) 
    {
        return float3(1.f - (u.x() + u.y()) / u.z(), u.y() / u.z(), u.x() / u.z());
    }
    else
    {
        // in this case generate negative coordinates, it will be thrown away by the rasterizator
        return float3(-1, 1, 1);  
    }    
}

void BDevice::draw_triangle_shader(float3 a, float3 b, float3 c, BShader& shader)
{
    a = ((shader.cam)->vp_mat * float4(a)).no_w();
    b = ((shader.cam)->vp_mat * float4(b)).no_w();
    c = ((shader.cam)->vp_mat * float4(c)).no_w();

    // rasterizing
    int minx = static_cast<int>(min(a.x(), min(b.x(), c.x())));
    int miny = static_cast<int>(min(a.y(), min(b.y(), c.y())));
    int maxx = static_cast<int>(max(a.x(), max(b.x(), c.x())));
    int maxy = static_cast<int>(max(a.y(), max(b.y(), c.y())));

    for(int y = miny; y <= maxy; ++y)
    {
        for(int x = minx; x <= maxx; ++x)
        {
            float3 bary_coord = calc_bary(a, b, c, x, y);

            if(bary_coord[0] >= 0 && bary_coord[1] >= 0 && bary_coord[2] >= 0)
            {
                float3 color{};
                if(!shader.fragment_shader(bary_coord, color))
                {
                    /* projection correct interp z */
                    float z = 1 / dot(bary_coord, float3(1 / a.z(), 1 / b.z(), 1 / c.z()));
                    if(z > z_buffer[x + y * mwidth])
                    {
                        z_buffer[x + y * mwidth] = z;
                        draw_point(x, y, color);
                    } 
                }                              
            }
        }
    }
}

void BDevice::draw_triangle_shadow(float3 a, float3 b, float3 c, BShader& shader)
{
    a = ((shader.cam)->vp_mat * float4(a)).no_w();
    b = ((shader.cam)->vp_mat * float4(b)).no_w();
    c = ((shader.cam)->vp_mat * float4(c)).no_w();

    // rasterizing
    int minx = static_cast<int>(min(a.x(), min(b.x(), c.x())));
    int miny = static_cast<int>(min(a.y(), min(b.y(), c.y())));
    int maxx = static_cast<int>(max(a.x(), max(b.x(), c.x())));
    int maxy = static_cast<int>(max(a.y(), max(b.y(), c.y())));

    for(int y = miny; y <= maxy; ++y)
    {
        for(int x = minx; x <= maxx; ++x)
        {
            float3 bary_coord = calc_bary(a, b, c, x, y);

            if(bary_coord[0] >= 0 && bary_coord[1] >= 0 && bary_coord[2] >= 0)
            {
                /* projection correct interp z */
                float z = 1 / dot(bary_coord, float3(1 / a.z(), 1 / b.z(), 1 / c.z()));
                if(z > shadow_buffer[x + y * mwidth])
                {
                    shadow_buffer[x + y * mwidth] = z;
                }                             
            }
        }
    }
}
//----------------------------------------------------------------------
// TESTs
//----------------------------------------------------------------------
void BDevice::TEST_show_model_wireframe()
{
    std::string path = "models/african_head/";
    BModel* model = new BModel(path + "african_head.obj");

    BCamera camera(mwidth, mheight);

    while(!window_close)
    {
        refresh_z();
        refresh_real_buffer();

        camera.update(window.coord_delta, window.mouse_scroll);
        window.coord_delta = int2(0, 0);
        window.mouse_scroll = 0;
        
        for(size_t i = 0; i < model->nfaces(); ++i)
        {
            float3 va = model->vert(i, 0); 
            float3 vb = model->vert(i, 1); 
            float3 vc = model->vert(i, 2); 

            float3 sva = camera.to_screen(va);
            float3 svb = camera.to_screen(vb);
            float3 svc = camera.to_screen(vc);

            int x0 = static_cast<int>(sva.x());
            int y0 = static_cast<int>(sva.y());
            int x1 = static_cast<int>(svb.x());
            int y1 = static_cast<int>(svb.y());
            int x2 = static_cast<int>(svc.x());
            int y2 = static_cast<int>(svc.y());

            draw_line(int2(x0, y0), int2(x1, y1));
            draw_line(int2(x0, y0), int2(x2, y2));
            draw_line(int2(x1, y1), int2(x2, y2));
        }
        
        copy_and_poll_event();
    }
    destroy_buffers();
    delete model;
}

void BDevice::TEST_show_shadow_shader()
{

    std::string path = "models/african_head/";
    BModel* model = new BModel(path + "african_head.obj");
    if(!model->load_texture(path + "african_head_diffuse.tga", 0))
        std::cout << "load diffuse texture failed." << std::endl;
    else
        std::cout << "load diffuse texture success." << std::endl;

    if(!model->load_texture(path + "african_head_nm.png", 1))
        std::cout << "load norm texture failed." << std::endl;
    else
        std::cout << "load norm texture success." << std::endl;

    if(!model->load_texture(path + "african_head_spec.tga", 2)) //
        std::cout << "load specular texture failed." << std::endl;
    else
        std::cout << "load specular texture success." << std::endl;

    // lights and cams
    float3 light_pos = float3(1.0f);
    BCamera camera(mwidth, mheight);
    BCamera scam(camera.width, camera.height, camera.fov, light_pos, -light_pos);
    
    // shaders
    BlinnPhongShadowShader_world main_shader;
    main_shader.model = model;
    main_shader.cam = &camera;
    main_shader.scam = &scam;
    main_shader.shadow_buffer = shadow_buffer;

    ShadowShader_world shadow_shader;
    shadow_shader.model = model;
    shadow_shader.cam = &scam;
      
    // main loop  
    window.show();
    while(!window_close)
    {
        refresh_z();
        refresh_real_buffer();

        camera.update(window.coord_delta, window.mouse_scroll);
        window.coord_delta = int2(0, 0);
        window.mouse_scroll = 0;
                
        //shadow pass
        for(size_t i = 0; i < model->nfaces(); ++i)
        {
            float3 pva = shadow_shader.vertex_shader(i, 0);
            float3 pvb = shadow_shader.vertex_shader(i, 1);
            float3 pvc = shadow_shader.vertex_shader(i, 2);

            /* backface culling in projection */
            float3 norm = cross(pvb - pva, pvc - pva);
            if(norm.z() < 0)  
            {
                continue;  //culling                 
            } 
            else
            {
                draw_triangle_shadow(pva, pvb, pvc, shadow_shader); 
            }    
        } 

        //main pass
        for(size_t i = 0; i < model->nfaces(); ++i)
        {
            float3 pva = main_shader.vertex_shader(i, 0);
            float3 pvb = main_shader.vertex_shader(i, 1);
            float3 pvc = main_shader.vertex_shader(i, 2);

            /* backface culling in projection*/
            float3 norm = cross(pvb - pva, pvc - pva);
            if(norm.z() < 0)  
            {
                continue;  //culling                 
            } 
            else
            {
                draw_triangle_shader(pva, pvb, pvc, main_shader); 
            }    
        }
        copy_and_poll_event();
    }
    delete model;
    destroy_buffers();
    
}

void BDevice::TEST_show_model_shader()
{
    std::string path = "models/diablo_pose/";
    BModel* model = new BModel(path + "diablo_pose.obj");
    if(!model->load_texture(path + "diablo_pose_diffuse.tga", 0))
        std::cout << "load diffuse texture failed." << std::endl;
    else
        std::cout << "load diffuse texture success." << std::endl;

    if(!model->load_texture(path + "diablo_pose_nm.tga", 1))
        std::cout << "load norm texture failed." << std::endl;
    else
        std::cout << "load norm texture success." << std::endl;

    if(!model->load_texture(path + "diablo_pose_spec.tga", 2)) //
        std::cout << "load specular texture failed." << std::endl;
    else
        std::cout << "load specular texture success." << std::endl;
  
    // lights and cams
    float3 light_pos = float3(1.0f);
    BCamera camera(mwidth, mheight);
    
    // shaders
    BlinnPhongShader_world main_shader;
    main_shader.model = model;
    main_shader.cam = &camera;
      
    // main loop  
    window.show();
    while(!window_close)
    {
        refresh_z();
        refresh_real_buffer();

        camera.update(window.coord_delta, window.mouse_scroll);
        window.coord_delta = int2(0, 0);
        window.mouse_scroll = 0;
                
        //main pass
        for(size_t i = 0; i < model->nfaces(); ++i)
        {
            float3 pva = main_shader.vertex_shader(i, 0);
            float3 pvb = main_shader.vertex_shader(i, 1);
            float3 pvc = main_shader.vertex_shader(i, 2);

            /* backface culling in projection*/
            float3 norm = cross(pvb - pva, pvc - pva);
            if(norm.z() < 0)  
            {
                continue;  //culling                 
            } 
            else
            {
                draw_triangle_shader(pva, pvb, pvc, main_shader); 
            }    
        }
        copy_and_poll_event();
    }
    delete model;
    destroy_buffers();
    
}