///////////////////////////////////////////////////////////////
// Date: 18.02.2025
// Author: NS_Deathman
// Deathman's samples mystery box
///////////////////////////////////////////////////////////////
// SDL Simply window creating sample
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
// SDL Includes (You need to add $(SolutionDir)\Third-Party\Include\
// to VS include paths of your project) 
#include <SDL/SDL.h>
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
SDL_Renderer* g_Renderer = nullptr;
///////////////////////////////////////////////////////////////
// Window procedure function definition
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
    case WM_DESTROY:
        PostQuitMessage(NULL); // Post a quit message when the window is destroyed
        return NULL;
    }

    // Call default handler for other messages
    return DefWindowProc(hWnd, message, wParam, lParam);
}
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
    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
        return -1;
    }

    // Create an SDL window
    g_Window = SDL_CreateWindow("SDL2 Sample", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    if (g_Window == nullptr) 
    {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    // Create a rendering context
    g_Renderer = SDL_CreateRenderer(g_Window, -1, SDL_RENDERER_ACCELERATED);
    if (g_Renderer == nullptr) 
    {
        std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(g_Window);
        SDL_Quit();
        return -1;
    }

    //-------------------EVENT LOOP CREATING CODE-------------------//
    // Event loop
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

        // Set the drawing color
        SDL_SetRenderDrawColor(g_Renderer, r, g, b, 255); // RGBA
        SDL_RenderClear(g_Renderer); // Clear the screen

        // Draw things here...

        SDL_RenderPresent(g_Renderer); // Present the backbuffer
    }

    //-------------------APPLICATION DESTROYING CODE-------------------//

    // Clean up resources
    std::cout << "Ending event loop \n";
    std::cout << "Cleaning up resources...\n";

    SDL_DestroyRenderer(g_Renderer);
    SDL_DestroyWindow(g_Window);
    SDL_Quit();

    return 0;
}
///////////////////////////////////////////////////////////////
