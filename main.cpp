#include "BWindow.h"
#include "BDevice.h"

#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

int main()
{
    BDevice device(720, 720);
    /* wire frame african head  */
    //device.TEST_show_model_wireframe();

    /* Phong shading diablo */
    device.TEST_show_model_shader();
    
    /* hard shadow african head */
    //device.TEST_show_shadow_shader();
    
    return 0;
}