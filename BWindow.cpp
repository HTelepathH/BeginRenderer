#include "BWindow.h"

void handle_mouse_coord(BWindow* window, int x, int y, int pressed)
{
    if(pressed)
    {
        (window->mouse_coord).x = x;
        (window->mouse_coord).y = y;  
    }
    else
    {
        (window->coord_delta).x = x - (window->mouse_coord).x;
        (window->coord_delta).y = y - (window->mouse_coord).y;
        (window->mouse_coord).x = x;
        (window->mouse_coord).y = y;         
    }

}

void handle_mouse_coord(BWindow* window, int scroll)
{
    window->mouse_scroll = scroll;
}

LRESULT CALLBACK process_message(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    BWindow *window = (BWindow*)GetProp(hwnd, TEXT("ENTRY"));
    switch (message)
    {
        /*
    case WM_PAINT:
        paint_get_dc(hwnd);
    //    paint_begin_paint(hwnd);
        return 0;
*/  
    case WM_MOUSEMOVE:
    {
        int buttons = (int)wParam;
        if(buttons & MK_LBUTTON)
        {
            int coord_x = (int)LOWORD(lParam);
            int coord_y = (int)HIWORD(lParam);
            handle_mouse_coord(window, coord_x, coord_y, 0);
        }
    }
    case WM_LBUTTONDOWN:
    {
        int coord_x = (int)LOWORD(lParam);
        int coord_y = (int)HIWORD(lParam);
        int buttons = (int)wParam;
        handle_mouse_coord(window, coord_x, coord_y, 1);
        //std::cout << (window->coord_delta) << std::endl;
        break;
    }
/*     case WM_LBUTTONUP:
    {
        int coord_x = (int)LOWORD(lParam);
        int coord_y = (int)HIWORD(lParam);
        //int buttons = (int)wParam;
        handle_mouse_coord(window, coord_x, coord_y, 0);
        //std::cout << (window->coord_delta) << std::endl;
        break;
    } */
    case WM_MOUSEWHEEL:
    {
        short scroll = (short)HIWORD(wParam);
        handle_mouse_coord(window, scroll);
        //std::cout << (window->mouse_scroll) << std::endl;
        break;
    }
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

BWindow::BWindow(size_t w, size_t h) :
    mheight(h), mwidth(w)
{
    register_class();
    // adjust display window
    DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = mwidth;
    rect.bottom = mheight;
    AdjustWindowRect(&rect, style, 0);
    mwidth = rect.right - rect.left;
    mheight = rect.bottom - rect.top;

    handle = CreateWindow(TEXT("window_class"),                  // window class name
        TEXT("Begin"),              // window caption
        WS_OVERLAPPEDWINDOW,        // window style
        CW_USEDEFAULT,              // initial x position
        CW_USEDEFAULT,              // initial y position
        mwidth,                      // initial x size
        mheight,                     // initial y size
        NULL,                       // parent window handle
        NULL,                       // window menu handle
        GetModuleHandle(NULL),     // program instance handle
        NULL);                     // creation parameters

}

BWindow::~BWindow()
{

}

void BWindow::register_class()
{
    ATOM class_atom;
    WNDCLASS window_class;
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = process_message;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = GetModuleHandle(NULL);
    window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    window_class.lpszMenuName = NULL;
    window_class.lpszClassName =  TEXT("window_class");
    class_atom = RegisterClass(&window_class);
}

void BWindow::show()
{
    ShowWindow(handle, SW_SHOW);
}
