#ifndef BWINDOW_H
#define BWINDOW_H

#include <windows.h>
#include <string.h>
#include <ctime>
#include <iostream>
#include "math/vec3.h"

class BWindow
{
public:
    BWindow() = default;
    BWindow(size_t w, size_t h);
    ~BWindow();

    void show();
    size_t height() const { return mheight; }
    size_t width() const { return mwidth; }

    int2 mouse_coord;
    int2 coord_delta = int2(0, 0);
    short mouse_scroll = 0;
    HWND handle;

private:
    size_t mheight;
    size_t mwidth;

    void register_class();
    
};

#endif