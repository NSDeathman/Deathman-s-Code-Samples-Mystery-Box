///////////////////////////////////////////////////////////////
// Date: 18.02.2025
// Author: NS_Deathman
// Deathman's samples mystery box
///////////////////////////////////////////////////////////////
// SDL with DirectX9 combining sample
///////////////////////////////////////////////////////////////
// Windows includes (You need to add $(WindowsSDK_IncludePath)
// to VS include paths of your project)
#include <iostream>
#include <ctime>
#include <windows.h>
///////////////////////////////////////////////////////////////
// Windows libraries
#pragma comment( lib, "winmm.lib")
///////////////////////////////////////////////////////////////
// Includes (You need to add $(DXSDK_DIR)Include\ to VS 
// include paths of your project) 
#include <d3dx9.h>
///////////////////////////////////////////////////////////////
// Libraries (You need to add 
// $(DXSDK_DIR)Lib\$(LibrariesArchitecture)\ to VS include 
// paths of your project) 
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
///////////////////////////////////////////////////////////////
// SDL Includes (You need to add $(SolutionDir)\Third-Party\Include\
// to VS include paths of your project) 
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
///////////////////////////////////////////////////////////////
// SDL Libraries (You need to add 
// $(SolutionDir)\Third-Party\Libraries\$(LibrariesArchitecture)\
// to VS include paths of your project) 
#pragma comment(lib, "SDL2.lib")
// Add 
// copy /Y "$(SolutionDir)\Third-Party\Libraries\$(LibrariesArchitecture)\SDL2.dll" "$(OutDir)SDL2.dll"
// To your post build events for auto copying dll to your bin folder
///////////////////////////////////////////////////////////////
// Global variables
SDL_Window* g_Window = nullptr;
SDL_SysWMinfo g_WindowInfo;
HWND g_DefaultWindow = nullptr;
IDirect3D9* g_Direct3D = nullptr;
IDirect3DDevice9* g_Direct3DDevice = nullptr;
///////////////////////////////////////////////////////////////
// Entry point of the application
INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, INT) 
{
    //-------------------EXTERNAL CONSOLE LOG WINDOW CREATING CODE-------------------//
    if (AllocConsole())
    {
        // Use _CRT_SECURE_NO_WARNINGS define in VS project settings
        (void)freopen("CONIN$", "r", stdin);
        (void)freopen("CONOUT$", "w", stderr);
        (void)freopen("CONOUT$", "w", stdout);

        SetConsoleCP(1251);
        SetConsoleOutputCP(1251);
    }

    _set_error_mode(_OUT_TO_STDERR);

    std::cout << "Console log window created successfully \n";
    std::cout << "\n";

    std::cout << "Starting application\n";

    //-------------------SDL INITIALIZING CODE-------------------//
    std::cout << "\n";
    std::cout << "Initializing SDL \n";

    // Create an SDL window
    std::cout << "Initializing SDL Window \n";
    g_Window = SDL_CreateWindow("SDL2 with DX9 Sample", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    if (g_Window == nullptr) 
    {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    // Get SDL version
    SDL_VERSION(&g_WindowInfo.version);

    // Result value for handle function execution result
    HRESULT result = E_FAIL;

    // Get window info for get hwnd window
    result = SDL_GetWindowWMInfo(g_Window, &g_WindowInfo);

    // Get default HWND window for DirectX
    g_DefaultWindow = g_WindowInfo.info.win.window;

    //-------------------DIRECT3D CREATING CODE-------------------//
    // Initialize Direct3D
    std::cout << "\n";
    std::cout << "Creating Direct3D 9 \n";
    g_Direct3D = Direct3DCreate9(D3D_SDK_VERSION);

    D3DPRESENT_PARAMETERS Direct3DPresentationParams;
    ZeroMemory(&Direct3DPresentationParams, sizeof(Direct3DPresentationParams));
    Direct3DPresentationParams.Windowed = TRUE;
    Direct3DPresentationParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    Direct3DPresentationParams.BackBufferFormat = D3DFMT_UNKNOWN;
    Direct3DPresentationParams.EnableAutoDepthStencil = TRUE;
    Direct3DPresentationParams.AutoDepthStencilFormat = D3DFMT_D24X8;

    // Create the device
    std::cout << "Creating Direct3D 9 Device \n";
    result = g_Direct3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        g_DefaultWindow,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &Direct3DPresentationParams,
        &g_Direct3DDevice
    );

    if (FAILED(result))
    {
        // Handle device creation failure (add your error handling here)
        std::cout << "Error in Direct3D 9 Device creating procedure \n";
        SDL_DestroyWindow(g_Window);
        SDL_Quit();
        return NULL;
    }

    //-------------------EVENT LOOP CREATING CODE-------------------//
    // Event loop
    std::cout << "\n";
    std::cout << "Starting event loop \n";
    bool running = true;
    SDL_Event event;

    while (running) 
    {
        // Handle events
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT) 
            {
                running = false;
            }
        }

        // Clear the screen with a color that changes over time
        Uint32 Time = SDL_GetTicks(); // Get the current time in milliseconds

        // Generating color values between 0-255 using sine functions
        uint8_t r = (uint8_t)((sin(Time * 0.0005f) + 1.0f) * 127.5f);
        uint8_t g = (uint8_t)((sin(Time * 0.0025f) + 1.0f) * 127.5f);
        uint8_t b = (uint8_t)((sin(Time * 0.0001f) + 1.0f) * 127.5f);

        // Clearing
        g_Direct3DDevice->Clear(NULL, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB((int)r, (int)g, (int)b), 1.0f, NULL);

        // Begin the scene
        g_Direct3DDevice->BeginScene();

        //Rendering code

        // End the scene
        g_Direct3DDevice->EndScene();

        // Present frame to screen
        g_Direct3DDevice->Present(NULL, NULL, NULL, NULL);
    }

    //-------------------APPLICATION DESTROYING CODE-------------------//

    // Clean up resources
    std::cout << "\n";
    std::cout << "Ending event loop \n";
    std::cout << "Cleaning up resources...\n";

    if (g_Direct3DDevice)
    {
        std::cout << "Releasing Direct3D Device \n";
        g_Direct3DDevice->Release();
        g_Direct3DDevice = nullptr;
    }

    if (g_Direct3D)
    {
        std::cout << "Releasing Direct3D \n";
        g_Direct3D->Release();
        g_Direct3D = nullptr;
    }

    SDL_DestroyWindow(g_Window);
    SDL_Quit();

    return 0;
}
///////////////////////////////////////////////////////////////
