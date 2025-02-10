// #############################################################################
//                           Platform Globals
// #############################################################################

static bool running= true;

// #############################################################################
//                           Cross Platform functions
// #############################################################################

bool platform_create_window(int width, int height, char* title);
void platform_update_window();

// #############################################################################
//                           Windows Platform
// #############################################################################

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>


// #############################################################################
//                           Windows Globals
// #############################################################################

static HWND window;
// #############################################################################
//                           Platform Implementation
// #############################################################################

bool platform_create_window(int width, int height, char* title)
{
    HINSTANCE instance = GetModuleHandleA(0);

    WNDCLASSA wc={};
    wc.hInstance = instance;
    wc.hIcon=LoadIcon(instance,IDI_APPLICATION);
    wc.hCursor=LoadCursor(instance,IDC_ARROW); // This means we decide the look of the cursor
    wc.lpszClassName=title; //This is not the title, just a unique indentifier
    wc.lpfnWndProc=DefWindowProcA; //Callback for Input into the Window

    // https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexa
    if(!RegisterClassA(&wc))
    {
        return false;
    }
    //WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
    int dwStyle = WS_OVERLAPPEDWINDOW;

    window = CreateWindowExA(0,title,
                                  title,
                                  dwStyle,
                                  100,
                                  100,
                                  width,
                                  height, 
                                  NULL,
                                  NULL,
                                  instance,
                                  NULL);
    if(!window)
    {
        return false;
    }

    ShowWindow(window,SW_SHOW);

    return true;
}

void platform_update_window()
{
    MSG msg;
    
    while (PeekMessageA(&msg,window,0,0,PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg); // calls the callback specified when creating the window
    }
}
#endif

int main()
{
    platform_create_window(1280,720, "ALAN GAME");
    while(1)
    {
        //update
        //platform_update_window();
    }
    return 0;
}