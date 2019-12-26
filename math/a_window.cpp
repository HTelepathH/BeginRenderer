#include <windows.h>
#include <iostream>
#include <string.h>
#include <cmath>
#include <ctime>

#define j2h(x) (3.0 * (x) / 180.0)

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

void paint_get_dc(HWND hwnd);
void paint_begin_paint(HWND hwnd);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR szCmdLine, int iCmdShow)
{
//-----------------------------------------------------------------
    WNDCLASS     wndclass;
    // visual
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);//MAKEINTRESOURCE(ID_ICON1)
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
    // system
    wndclass.hInstance = hInstance;    
    wndclass.lpfnWndProc = WndProc;
    wndclass.lpszClassName = TEXT("HelloWin");
    // extra
    wndclass.style = NULL;
    wndclass.lpszMenuName = NULL;
    wndclass.cbClsExtra = NULL;
    wndclass.cbWndExtra = NULL;
//-----------------------------------------------------------------
    
    if(!RegisterClass(&wndclass))
    {
        return -1;
    } 

    HWND hWindow;
    hWindow = CreateWindow(TEXT("HelloWin"),                  // window class name
        TEXT("The Hello Program"), // window caption
        WS_OVERLAPPEDWINDOW,        // window style
        CW_USEDEFAULT,              // initial x position
        CW_USEDEFAULT,              // initial y position
        1280,                       // initial x size
        720,                        // initial y size
        NULL,                       // parent window handle
        NULL,                       // window menu handle
        hInstance,                  // program instance handle
        NULL);                     // creation parameters

    ShowWindow(hWindow, iCmdShow);
    UpdateWindow(hWindow);


    MSG msg;
/*
    while (GetMessage(&msg, NULL, 0, 0))
    {   
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
*/
    while(1)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            paint_begin_paint(hWindow);
            Sleep(1);
        }
    }

    return 0; 
}

void paint_get_dc(HWND hwnd)
{
    HDC hdc;
    HDC mdc;
    HBITMAP hbmp; // 位图绘制对象句柄
 
    hdc = GetDC(hwnd);
    // 创建缓存DC (当前窗口DC的兼容DC)
    mdc = CreateCompatibleDC(hdc);
    
    
    BITMAPINFOHEADER bi_header;
    bi_header.biSize = sizeof(BITMAPINFOHEADER);
    bi_header.biWidth = 1280;
    bi_header.biHeight = -720;  /* top-down */
    bi_header.biPlanes = 1;
    bi_header.biBitCount = 32;
    bi_header.biCompression = BI_RGB;

    unsigned char* dummy;
    hbmp = CreateDIBSection(mdc, (BITMAPINFO*)&bi_header,
                                  DIB_RGB_COLORS, (void**)&dummy, NULL, 0);

    for(size_t r = 0; r < 720; ++r)
    {
        for(size_t c = 0; c < 1280; ++c)
        {
            time_t seconds = time(NULL);
            size_t index = r * 1280 * 4 + c * 4;
            unsigned char* pixel = &(dummy[index]);
            pixel[0] = static_cast<unsigned char>(255 * (sin(5 * j2h(seconds)) + 1) / 2);
            pixel[1] = static_cast<unsigned char>(255 * (sin(5 * j2h(seconds)) + 1) / 2);
            pixel[2] = static_cast<unsigned char>(255 * (sin(5 * j2h(seconds)) + 1) / 2);
        }
    }

    // 缓存DC选择位图绘制对象（可以理解为将图片存到mdc中）
    SelectObject(mdc, hbmp);
 
    // 将缓存DC中的位图复制到窗口DC上
    BitBlt(
        hdc,            // 目的DC
        0,0,            // 目的DC的 x,y 坐标
        1280,           // 要粘贴的图片宽
        720,            // 要粘贴的图片高
        mdc,            // 缓存DC
        0,0,            // 缓存DC的 x,y 坐标
        SRCCOPY         // 粘贴方式
        );

    ReleaseDC(hwnd, hdc);
    DeleteObject(hbmp);
    DeleteDC(mdc);
    
}

void paint_begin_paint(HWND hwnd)
{
    PAINTSTRUCT ps;
    HDC hdc;
    HDC mdc;
    HBITMAP hbmp; // 位图绘制对象句柄
 
    hdc = BeginPaint(hwnd, &ps); 
 
    // 创建缓存DC (当前窗口DC的兼容DC)
    mdc = CreateCompatibleDC(hdc);
    
    BITMAPINFOHEADER bi_header;
    bi_header.biSize = sizeof(BITMAPINFOHEADER);
    bi_header.biWidth = 1280;
    bi_header.biHeight = -720;  /* top-down */
    bi_header.biPlanes = 1;
    bi_header.biBitCount = 32;
    bi_header.biCompression = BI_RGB;

    unsigned char* dummy;
    hbmp = CreateDIBSection(mdc, (BITMAPINFO*)&bi_header,
                                  DIB_RGB_COLORS, (void**)&dummy, NULL, 0);

    for(size_t r = 0; r < 720; ++r)
    {
        for(size_t c = 0; c < 1280; ++c)
        {
            time_t seconds = time(NULL);
            size_t index = r * 1280 * 4 + c * 4;
            unsigned char* pixel = &(dummy[index]);
            pixel[0] = static_cast<unsigned char>(255 * (sin(5 * j2h(seconds)) + 1) / 2);
            pixel[1] = static_cast<unsigned char>(255 * (sin(5 * j2h(seconds)) + 1) / 2);
            pixel[2] = static_cast<unsigned char>(255 * (sin(5 * j2h(seconds)) + 1) / 2);
        }
    }

    // 缓存DC选择位图绘制对象（可以理解为将图片存到mdc中）
    SelectObject(mdc, hbmp);
 
    // 将缓存DC中的位图复制到窗口DC上
    BitBlt(
        hdc,            // 目的DC
        0,0,            // 目的DC的 x,y 坐标
        1280,           // 要粘贴的图片宽
        720,            // 要粘贴的图片高
        mdc,            // 缓存DC
        0,0,            // 缓存DC的 x,y 坐标
        SRCCOPY         // 粘贴方式
        );
 
    DeleteObject(hbmp);
    DeleteDC(mdc);
    EndPaint(hwnd, &ps);
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        /*
    case WM_PAINT:
        paint_get_dc(hwnd);
    //    paint_begin_paint(hwnd);
        return 0;
*/
    case WM_LBUTTONDOWN:
        MessageBox(hwnd, TEXT("left click"), TEXT("Hint"), MB_OK);
        return 0;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}