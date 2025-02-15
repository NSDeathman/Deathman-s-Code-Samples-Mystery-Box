///////////////////////////////////////////////////////////////
// Date: 15.02.2025
// Author: NS_Deathman
// Deathman's samples mystery box
///////////////////////////////////////////////////////////////
// Lesson ¹2 - "DirectX 9 device reseting"
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
// Global variables
IDirect3D9* g_Direct3D = nullptr;
IDirect3DDevice9* g_Direct3DDevice = nullptr;
HWND g_Window = nullptr;

bool g_bDeviceLost = false;
bool g_bNeedReset = false;
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
    //-------------------WINDOW CREATING CODE-------------------//
    // Register the window class 
    // (For correct work turn off worldwide and unicode symbols using 
    // in VS project settings)
    LPCSTR window_class_name = (LPCSTR)"DirectXSampleWindowClass";
    LPCSTR window_name = (LPCSTR)"DirectX 9 Sample";

    WNDCLASS WindClass = {};
    WindClass.lpfnWndProc = WindowProcedure;           // Set the window procedure
    WindClass.hInstance = hInstance;                   // Handle to the application instance
    WindClass.lpszClassName = window_class_name;       // Name of the window class
    WindClass.hCursor = LoadCursor(NULL, IDC_ARROW);   // Default cursor

    // Register the window class
    RegisterClass(&WindClass);

    // Create the window
    g_Window = CreateWindowEx(
        NULL,                       // Optional window styles
        window_class_name,          // Window class
        window_name,                // Window text
        WS_OVERLAPPEDWINDOW,        // Window style
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, // Size and position
        NULL,                       // Parent window
        NULL,                       // Menu
        hInstance,                  // Instance handle
        NULL                        // Additional application data
    );

    if (g_Window == NULL)
    {
        // If the window creation failed, exit
        return NULL;
    }

    // Show the window 
    // (You need turn subsystem to Windows (/SUBSYSTEM:WINDOWS) 
    // in VS project settings for this)
    ShowWindow(g_Window, SW_SHOW);

    //-------------------DIRECT3D CREATING CODE-------------------//
    // Initialize Direct3D
    g_Direct3D = Direct3DCreate9(D3D_SDK_VERSION);

    D3DPRESENT_PARAMETERS Direct3DPresentationParams;
    ZeroMemory(&Direct3DPresentationParams, sizeof(Direct3DPresentationParams));
    Direct3DPresentationParams.Windowed = TRUE;
    Direct3DPresentationParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    Direct3DPresentationParams.BackBufferFormat = D3DFMT_UNKNOWN;
    Direct3DPresentationParams.EnableAutoDepthStencil = TRUE;
    Direct3DPresentationParams.AutoDepthStencilFormat = D3DFMT_D24X8;

    // Result value for handle function execution result
    HRESULT result = E_FAIL;

    // Create the device
    result = g_Direct3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        g_Window, // Pass the window handle here
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &Direct3DPresentationParams,
        &g_Direct3DDevice
    );

    if (FAILED(result))
    {
        // Handle device creation failure (add your error handling here)
        return NULL;
    }

    //-------------------EVENT LOOP PROCESSING CODE-------------------//
    MSG WindowMessage;
    while (true)
    {
        // Process window messages
        PeekMessage(&WindowMessage, NULL, NULL, NULL, PM_REMOVE);
        TranslateMessage(&WindowMessage);
        DispatchMessage(&WindowMessage);

        // Break out of the loop if the window is closed
        if (WindowMessage.message == WM_QUIT)
            break;

        // Device lost event handling
        if (g_bDeviceLost)
        {
            result = g_Direct3DDevice->TestCooperativeLevel();

            if (result == D3DERR_DEVICELOST)
                Sleep(10);

            if (result == D3DERR_DEVICENOTRESET)
                g_bNeedReset = true;

            g_bDeviceLost = false;
        }

        // Device reseting code
        if (g_bNeedReset)
        {
            result = g_Direct3DDevice->Reset(&Direct3DPresentationParams);
                
            if (result == D3DERR_INVALIDCALL)
                break;//ERROR_MESSAGE("Invalid call while device resetting");

            g_bNeedReset = false;
        }

        // Clear the back buffer
        // Generating new color every frame with a time
        D3DXVECTOR4 ClearColor = { NULL, NULL, NULL, NULL };

        // Get actual time
        DWORD Time = timeGetTime();

        // Generating {0; 1} value of RGBA color with sin and cos
        ClearColor.x = sinf(Time * 0.0005f);
        ClearColor.y = sinf(Time * 0.0025f);
        ClearColor.z = sinf(Time * 0.0001f);

        // Set alpha to 1.0f (set untransparent color)
        ClearColor.w = 1.0f;

        // Transform {0; 1} color to {0; 255} value
        ClearColor *= 255;

        // Clearing
        g_Direct3DDevice->Clear(NULL, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA((int)ClearColor.x, (int)ClearColor.y, (int)ClearColor.z, (int)ClearColor.w), 1.0f, NULL);

        // Begin the scene
        g_Direct3DDevice->BeginScene();

        //Rendering code

        // End the scene
        g_Direct3DDevice->EndScene();

        // Present frame to screen
        result = g_Direct3DDevice->Present(NULL, NULL, NULL, NULL);

        // Set device lost event flag true if Present execution result return it
        if (result == D3DERR_DEVICELOST)
            g_bDeviceLost = true;
    }

    //-------------------DESTROYING CODE-------------------//
    if (g_Direct3DDevice) 
    {
        g_Direct3DDevice->Release();
        g_Direct3DDevice = nullptr;
    }

    if (g_Direct3D) 
    {
        g_Direct3D->Release();
        g_Direct3D = nullptr;
    }

    // Return the message parameter
    return (INT)WindowMessage.wParam;
}
///////////////////////////////////////////////////////////////
