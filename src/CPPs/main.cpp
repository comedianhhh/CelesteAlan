#include "../Headers/Alan_lib.h"
#include "../Headers/platform.h"

#define APIENTRY
#include"glcorearb.h"


#ifdef _WIN32
#include "win32_platform.cpp"
#endif

#include"../Headers/gl_renderer.h"
int main()
{
    platform_create_window(1280,720, "ALAN GAME");
    while(running)
    {
        //update
        platform_update_window();
        SM_ERROR("This is an error message %d", 5);
        SM_WARN("This is a warning message %d", 5);
        SM_TRACE("This is a trace message %d", 5);
        SM_ASSERT(false,"Assertion not Hit!");
        
        
    }
    return 0;
}

