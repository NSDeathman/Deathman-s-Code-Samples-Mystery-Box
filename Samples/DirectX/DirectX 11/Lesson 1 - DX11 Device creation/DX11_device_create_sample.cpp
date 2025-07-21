///////////////////////////////////////////////////////////////
// Date: 18.02.2025
// Author: NS_Deathman
// Deathman's samples mystery box
///////////////////////////////////////////////////////////////
// Lesson ¹1 - "DirectX 11 device and window creating"
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
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXcolors.h>
///////////////////////////////////////////////////////////////
using namespace DirectX;
///////////////////////////////////////////////////////////////
// Libraries (You need to add 
// $(DXSDK_DIR)Lib\$(LibrariesArchitecture)\ to VS include 
// paths of your project) 
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
///////////////////////////////////////////////////////////////
// Global variables
ID3D11Device* g_Direct3DDevice = nullptr;
ID3D11DeviceContext* g_Direct3DContext = nullptr;
IDXGISwapChain* g_SwapChain = nullptr;
ID3D11RenderTargetView* g_RenderTargetView = nullptr;
HWND g_Window = nullptr;
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

    //-------------------WINDOW CREATING CODE-------------------//
    // Register the window class 
    // (For correct work turn off worldwide and unicode symbols using 
    // in VS project settings)
    LPCSTR window_class_name = (LPCSTR)"DirectXSampleWindowClass";
    LPCSTR window_name = (LPCSTR)"DirectX 11 Sample";

    WNDCLASS WindClass = {};
    WindClass.lpfnWndProc = WindowProcedure;           // Set the window procedure
    WindClass.hInstance = hInstance;                   // Handle to the application instance
    WindClass.lpszClassName = window_class_name;       // Name of the window class
    WindClass.hCursor = LoadCursor(NULL, IDC_ARROW);   // Default cursor

    // Register the window class
    std::cout << "Window class registering \n";
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
        std::cout << "Error in window creating procedure \n";
        return NULL;
    }

    std::cout << "Window created successfully \n";

    // Show the window 
    // (You need turn subsystem to Windows (/SUBSYSTEM:WINDOWS) 
    // in VS project settings for this)
    ShowWindow(g_Window, SW_SHOW);

    //-------------------DIRECT3D CREATING CODE-------------------//
    // Initialize Direct3D
    
    DXGI_SWAP_CHAIN_DESC swapChainDescription = {};
    swapChainDescription.BufferCount = 1;
    swapChainDescription.BufferDesc.Width = 800;
    swapChainDescription.BufferDesc.Height = 600;
    swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDescription.OutputWindow = g_Window;
    swapChainDescription.SampleDesc.Count = 1;
    swapChainDescription.Windowed = TRUE;

    // Result value for handle function execution result
    HRESULT result = E_FAIL;

    D3D_FEATURE_LEVEL FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
    UINT numFeatureLevelsRequested = 1;
    D3D_FEATURE_LEVEL FeatureLevelsSupported;

    std::cout << "Creating Direct3D 11 Device and Swap Chain \n";
    result = D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        0,
        &FeatureLevelsRequested,
        numFeatureLevelsRequested,
        D3D11_SDK_VERSION,
        &swapChainDescription,
        &g_SwapChain,
        &g_Direct3DDevice,
        &FeatureLevelsSupported,
        &g_Direct3DContext);

    if (FAILED(result))
    {
        // Handle device creation failure (add your error handling here)
        std::cout << "Error in Direct3D 11 Device and Swap Chain creating procedure \n";
        return result;
    }

    std::cout << "Creating viewport \n";
    ID3D11Texture2D* pBackBuffer;

    // Get a pointer to the back buffer
    result = g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    // Create a render-target view
    g_Direct3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_RenderTargetView);

    // Bind the view
    g_Direct3DContext->OMSetRenderTargets(1, &g_RenderTargetView, NULL);

    // Setup the viewport
    D3D11_VIEWPORT Viewport;
    Viewport.Width = 800;
    Viewport.Height = 600;
    Viewport.MinDepth = 0.0f;
    Viewport.MaxDepth = 1.0f;
    Viewport.TopLeftX = 0;
    Viewport.TopLeftY = 0;
    g_Direct3DContext->RSSetViewports(1, &Viewport);

    //-------------------EVENT LOOP PROCESSING CODE-------------------//
    std::cout << "\n";
    std::cout << "Starting event loop \n";

    MSG WindowMessage;
    ZeroMemory(&WindowMessage, sizeof(WindowMessage));
    while (WindowMessage.message != WM_QUIT)
    {
        if (PeekMessage(&WindowMessage, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&WindowMessage);
            DispatchMessage(&WindowMessage);
        }

        // Clear the back buffer
        // Generating new color every frame with a time
        XMFLOAT4 ClearColor = { NULL, NULL, NULL, NULL };

        // Get actual time
        DWORD Time = timeGetTime();

        // Generating {0; 1} value of RGBA color with sin and cos
        ClearColor.x = (sinf(Time * 0.0005f) + 1.0f) * 0.5f;
        ClearColor.y = (sinf(Time * 0.0025f) + 1.0f) * 0.5f;
        ClearColor.z = (sinf(Time * 0.0001f) + 1.0f) * 0.5f;

        // Set alpha to 1.0f (set untransparent color)
        ClearColor.w = 1.0f;

        XMVECTORF32 vecColor = { ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w };

        // Clearing
        g_Direct3DContext->ClearRenderTargetView(g_RenderTargetView, vecColor);

        //Rendering code

        // Present frame to screen
        g_SwapChain->Present(NULL, NULL);
    }

    //-------------------DESTROYING CODE-------------------//
    std::cout << "\n";
    std::cout << "Ending event loop \n";
    std::cout << "\n";

    if (g_RenderTargetView)
    {
        std::cout << "Releasing Direct3D Render target \n";
        g_RenderTargetView->Release();
        g_RenderTargetView = nullptr;
    }

    if (g_SwapChain)
    {
        std::cout << "Releasing Direct3D Swap chain \n";
        g_SwapChain->Release();
        g_SwapChain = nullptr;
    }

    if (g_Direct3DContext)
    {
        std::cout << "Releasing Direct3D Context \n";
        g_Direct3DContext->Release();
        g_Direct3DContext = nullptr;
    }

    if (g_Direct3DDevice)
    {
        std::cout << "Releasing Direct3D Device \n";
        g_Direct3DDevice->Release();
        g_Direct3DDevice = nullptr;
    }

    ShowWindow(g_Window, SW_HIDE);

    std::cout << "\n";
    std::cout << "Application closed successfully, closing window after 3 seconds \n";
    Sleep(3000);

    // Return the message parameter
    return (INT)WindowMessage.wParam;
}
///////////////////////////////////////////////////////////////
