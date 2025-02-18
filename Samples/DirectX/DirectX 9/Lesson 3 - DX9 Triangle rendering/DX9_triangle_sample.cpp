///////////////////////////////////////////////////////////////
// Date: 17.02.2025
// Author: NS_Deathman
// Deathman's samples mystery box
///////////////////////////////////////////////////////////////
// Lesson ¹3 - "DirectX 9 triangle rendering"
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

int g_ResizeHeight = 0;
int g_ResizeWidth = 0;
///////////////////////////////////////////////////////////////
// Vertex buffer
LPDIRECT3DVERTEXBUFFER9 g_VertexBuffer = nullptr;

struct VERTEX_DATA 
{
    // The transformed position for the vertex
    D3DXVECTOR3 Position;

    // Color for the vertex
    D3DCOLOR Color;
};

#define VERTEXFVF (D3DFVF_XYZ | D3DFVF_DIFFUSE)
///////////////////////////////////////////////////////////////
// Window procedure function definition
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    switch (message) 
    {
    case WM_DESTROY:
        PostQuitMessage(NULL); // Post a quit message when the window is destroyed
        return NULL;
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
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
    LPCSTR window_name = (LPCSTR)"DirectX 9 Sample";

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
    std::cout << "Creating Direct3D 9 \n";
    g_Direct3D = Direct3DCreate9(D3D_SDK_VERSION);

    D3DPRESENT_PARAMETERS Direct3DPresentationParams;
    ZeroMemory(&Direct3DPresentationParams, sizeof(Direct3DPresentationParams));
    Direct3DPresentationParams.Windowed = TRUE;
    Direct3DPresentationParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    Direct3DPresentationParams.BackBufferFormat = D3DFMT_X8R8G8B8;
    Direct3DPresentationParams.EnableAutoDepthStencil = TRUE;
    Direct3DPresentationParams.AutoDepthStencilFormat = D3DFMT_D24X8;

    // Result value for handle function execution result
    HRESULT result = E_FAIL;

    // Create the device
    std::cout << "Creating Direct3D 9 Device \n";
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
        std::cout << "Error in Direct3D 9 Device creating procedure \n";
        return NULL;
    }

    //-------------------VERTEX BUFFER CREATING CODE-------------------//

    // Create and fill the vertex buffer
    std::cout << "\n";
    std::cout << "Creating vertex buffer \n";

    float VertexDataSize = sizeof(VERTEX_DATA);
    int TrianglesCount = 1;
    int VertecesCount = TrianglesCount * 3;
    float VertexBufferSize = VertexDataSize * VertecesCount;

    g_Direct3DDevice->CreateVertexBuffer(
        VertexBufferSize,           // Size of the vertex buffer
        NULL,                       // Usage
        VERTEXFVF,                  // FVF
        D3DPOOL_MANAGED,            // Memory pool
        &g_VertexBuffer,            // Pointer to the vertex buffer
        NULL                        // Handle to the resource
    );

    // Fill the vertex buffer with data
    VERTEX_DATA* VerticesData;
    std::cout << "Locking vertex buffer \n";
    result = g_VertexBuffer->Lock(NULL, sizeof(VerticesData), (void**)&VerticesData, NULL);

    if(FAILED(result))
        std::cout << "Error in vertex buffer locking procedure \n";

    // Define a triangle
    std::cout << "Fill vertex buffer \n";

    // Top vertex
    VerticesData[0].Position = { D3DXVECTOR3(0.0f, 1.0f, 0.0f) };
    VerticesData[0].Color = { D3DCOLOR_XRGB(255, 0, 0) };

    // Bottom right vertex
    VerticesData[1].Position = { D3DXVECTOR3(1.0f, -1.0f, 0.0f) };
    VerticesData[1].Color = { D3DCOLOR_XRGB(0, 255, 0) };

    // Bottom left vertex
    VerticesData[2].Position = { D3DXVECTOR3(-1.0f, -1.0f, 0.0f) };
    VerticesData[2].Color = { D3DCOLOR_XRGB(0, 0, 255) };

    std::cout << "Unlocking vertex buffer \n";
    result = g_VertexBuffer->Unlock();

    if (FAILED(result))
        std::cout << "Error in vertex buffer unlocking procedure \n";

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

        // Device lost event handling
        if (g_bDeviceLost)
        {
            std::cout << "Device was been lost - handle \n";

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
            std::cout << "Resetting Direct3D Device \n";

            result = g_Direct3DDevice->Reset(&Direct3DPresentationParams);
                
            if (result == D3DERR_INVALIDCALL)
            {
                std::cout << "Invalid call while device resetting \n";
                break;
            }

            g_bNeedReset = false;
        }

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            Direct3DPresentationParams.BackBufferWidth = g_ResizeWidth;
            Direct3DPresentationParams.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            g_bNeedReset = true;
        }

        // Clear the back buffer
        g_Direct3DDevice->Clear(NULL, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, NULL);

        // Begin the scene
        g_Direct3DDevice->BeginScene();

        // Set the vertex buffer to be active
        g_Direct3DDevice->SetStreamSource(0, g_VertexBuffer, 0, sizeof(VERTEX_DATA));

        // Set the FVF
        g_Direct3DDevice->SetFVF(VERTEXFVF);

        // Draw the triangle
        g_Direct3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, TrianglesCount);

        // Disable FFP lighting
        g_Direct3DDevice->SetRenderState(D3DRS_LIGHTING, false);

        // Set vertex color to output to screen
        g_Direct3DDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);

        // Set ambient lighting color 
        g_Direct3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(255, 255, 255, 255));

        // Set vertex color to diffuse color of material
        g_Direct3DDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);

        // End the scene
        g_Direct3DDevice->EndScene();

        // Present frame to screen
        result = g_Direct3DDevice->Present(NULL, NULL, NULL, NULL);

        // Set device lost event flag true if Present execution result return it
        if (result == D3DERR_DEVICELOST)
            g_bDeviceLost = true;
    }

    //-------------------DESTROYING CODE-------------------//
    std::cout << "\n";
    std::cout << "Ending event loop \n";
    std::cout << "\n";

    if (g_VertexBuffer) 
    {
        std::cout << "Releasing vertex buffer \n";
        g_VertexBuffer->Release();
        g_VertexBuffer = nullptr;
    }

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

    ShowWindow(g_Window, SW_HIDE);

    std::cout << "\n";
    std::cout << "Application closed successfully, closing window after 3 seconds \n";
    Sleep(3000);

    // Return the message parameter
    return (INT)WindowMessage.wParam;
}
///////////////////////////////////////////////////////////////
