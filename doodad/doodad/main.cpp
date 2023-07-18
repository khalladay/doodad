#include "common_header.h"
#include "doodad_app.h"
#include "win_init.h"
#include "input_handler.h"
#include <stdio.h>
#include <shlwapi.h>
#pragma comment (lib, "Shlwapi.lib")

/*
should spawn the window wherever your cursor is on screen when summoned via hook
need actual help menu
color output bg to have slightly different bg color for different outputs 
*/

void mainLoop();

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    HWND wndHdl = os::makeWindow(hInstance, TEXT(APP_NAME), DEFAULT_SCREEN_W, DEFAULT_SCREEN_H);

    char path[512];
    HMODULE hModule = GetModuleHandle(NULL);
    GetModuleFileNameA(hModule, path, 512);
    PathRemoveFileSpecA(path);

    App().Initialize(wndHdl, hInstance, path);

    mainLoop();

    return 0;

}

void mainLoop()
{
   App().Tick();
   App().Draw();

    while (1)
    {
        os::handleEvents();
        App().Tick();
        App().Draw();

        Sleep(16);
        if (App().ShouldExit())
        {
            break;
        }
    }
}
