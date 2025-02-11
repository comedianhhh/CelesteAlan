#include "Alan_lib.h"

#include "platform.h"

#include "input.h"

#define APIENTRY
#define GL_GLEXT_PROTOTYPES
#include"glcorearb.h"


#ifdef _WIN32
#include "win32_platform.cpp"
#endif
#include"gl_renderer.cpp"
int main()
{
    BumpAllocator transientStorage = make_bump_allocator(MB(50));
    platform_create_window(1280,720, "ALAN GAME");

    input.screenSizeX = 1280;
    input.screenSizeY = 720;

    gl_init(&transientStorage);
    while(running)
    {
        //update
        platform_update_window();
        gl_render();

        platform_sawp_buffers();
    }
    return 0;
}

