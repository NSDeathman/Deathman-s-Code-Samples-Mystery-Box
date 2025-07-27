///////////////////////////////////////////////////////////////
// Date: 18.02.2025
// Author: NS_Deathman
// Deathman's samples mystery box
///////////////////////////////////////////////////////////////
// SDL Simply window creating sample
///////////////////////////////////////////////////////////////
#include "stdafx.h"
///////////////////////////////////////////////////////////////
// Our input class including
#include "input.h"
///////////////////////////////////////////////////////////////
// Global variables
SDL_Window* g_Window = nullptr;
SDL_Renderer* g_Renderer = nullptr;
CInput* Input = nullptr;
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
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
        return -1;
    }

    // Create an SDL window
    g_Window = SDL_CreateWindow("SDL2 Sample", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 300, 300, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS);

    if (g_Window == nullptr) 
    {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    // Create a rendering context
    g_Renderer = SDL_CreateRenderer(g_Window, -1, SDL_RENDERER_PRESENTVSYNC);

    if (g_Renderer == nullptr) 
    {
        std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(g_Window);
        SDL_Quit();
        return -1;
    }

    // Initialize our input class
    Input = new CInput;

    //-------------------EVENT LOOP CREATING CODE-------------------//
    // Event loop
    std::cout << "\n";
    std::cout << "Starting event loop \n";
    bool running = true;
    SDL_Event Event;

    while (running) 
    {
        // Handle events
        while (SDL_PollEvent(&Event)) 
        {
            if (Event.type == SDL_QUIT)
                running = false;
        }

        // Update input every frame
        Input->OnFrame(Event);

        Input->SetNeedUpdateCursorWithGameController(true);

        Input->HandleCursorWithGameController();

        // Demo prints
        if(Input->KeyHolded(SDL_SCANCODE_W))
            std::cout << "W key holded \n";

        if (Input->KeyPressed(SDL_SCANCODE_A))
            std::cout << "A key pressed \n";

        if (Input->GamepadButtonHolded(SDL_CONTROLLER_BUTTON_A))
            std::cout << "Gamepad A button holded \n";

        if (Input->GamepadButtonPressed(SDL_CONTROLLER_BUTTON_B))
            std::cout << "Gamepad B button pressed \n";

        if (Input->KeyPressed(SDL_SCANCODE_ESCAPE))
        {
            std::cout << "Escape key pressed - closing application \n";
            running = false;
        }

        // Get left stick angle value in interval {-1; 1}
        float LeftStickX, LeftStickY = NULL;
        Input->GamepadGetLeftStick(LeftStickX, LeftStickY);

        if (LeftStickX != NULL || LeftStickY != NULL)
        {
            std::cout << "Gamepad left stick X " << LeftStickX << " Y " << LeftStickY;
            std::cout << "\n";
        }

        // Get right stick angle value in interval {-1; 1}
        float RightStickX, RightStickY = NULL;
        Input->GamepadGetRightStick(RightStickX, RightStickY);

        if (RightStickX != NULL || RightStickY != NULL)
        {
            std::cout << "Gamepad right stick X " << RightStickX << " Y " << RightStickY;
            std::cout << "\n";
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
    std::cout << "\n";
    std::cout << "Ending event loop \n";
    std::cout << "Cleaning up resources...\n";

    // Destroy our input class
    delete(Input);

    SDL_DestroyRenderer(g_Renderer);
    SDL_DestroyWindow(g_Window);
    SDL_Quit();

    return 0;
}
///////////////////////////////////////////////////////////////
