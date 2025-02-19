///////////////////////////////////////////////////////////////
// Date: 19.02.2025
// Author: NS_Deathman
// Deathman's samples mystery box
///////////////////////////////////////////////////////////////
// Lesson ¹5.5 - "Refactoring"
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
// Debug defines
//#define DEBUG_SHOW_WIREFRAME
//#define DEBUG_SHOW_VERTECES

#define SHOW_NORMALS 1
#define SHOW_UV 2
//#define DEBUG_SHOW_VERTEX_COLOR SHOW_NORMALS
///////////////////////////////////////////////////////////////
// Global variables
IDirect3D9* g_Direct3D = nullptr;
IDirect3DDevice9* g_Direct3DDevice = nullptr;
D3DPRESENT_PARAMETERS g_Direct3DPresentationParams = {};
HWND g_Window = nullptr;
///////////////////////////////////////////////////////////////
bool g_bNeedReset = false;
bool g_bNeedCloseApplication = false;
bool g_bNeedResizeWindow = false;
///////////////////////////////////////////////////////////////
int g_ScreenHeight = 800;
int g_ScreenWidth = 600;
int g_ScreenResizeHeight = 0;
int g_ScreenResizeWidth = 0;
///////////////////////////////////////////////////////////////
float g_ScreenResolutionAspectRatio = (float)g_ScreenWidth / (float)g_ScreenHeight;
///////////////////////////////////////////////////////////////
LPDIRECT3DTEXTURE9 g_Texture = nullptr;
///////////////////////////////////////////////////////////////
LPDIRECT3DVERTEXBUFFER9 g_VertexBuffer = nullptr;
LPDIRECT3DINDEXBUFFER9 g_IndexBuffer = nullptr;
///////////////////////////////////////////////////////////////
// Vertex attributes
struct VERTEX_DATA
{
    D3DXVECTOR3 Position;
    D3DXVECTOR3 Normal;
    D3DCOLOR Color;
    D3DXVECTOR2 UV;
};
#define VERTEXFVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1) 
///////////////////////////////////////////////////////////////
// Cube attrubutes
#define CUBE_VERTEX_COUNT 24
#define CUBE_INDEX_COUNT 36
#define CUBE_TRIANGLES_COUNT 12

D3DXVECTOR3 CubeVerticesPositions[CUBE_VERTEX_COUNT] =
{
    // Front face vertex positions
    D3DXVECTOR3(-1.0f, -1.0f, -1.0f),
    D3DXVECTOR3(-1.0f, 1.0f, -1.0f),
    D3DXVECTOR3(1.0f, 1.0f, -1.0f),
    D3DXVECTOR3(1.0f, -1.0f, -1.0f),

    // Back face vertex positions
    D3DXVECTOR3(1.0f, -1.0f, 1.0f),
    D3DXVECTOR3(1.0f, 1.0f, 1.0f),
    D3DXVECTOR3(-1.0f, 1.0f, 1.0f),
    D3DXVECTOR3(-1.0f, -1.0f, 1.0f),

    // Top face vertex positions
    D3DXVECTOR3(-1.0f, 1.0f, -1.0f),
    D3DXVECTOR3(-1.0f, 1.0f, 1.0f),
    D3DXVECTOR3(1.0f, 1.0f, 1.0f),
    D3DXVECTOR3(1.0f, 1.0f, -1.0f),

    // Bottom face vertex positions
    D3DXVECTOR3(-1.0f, -1.0f, -1.0f),
    D3DXVECTOR3(1.0f, -1.0f, -1.0f),
    D3DXVECTOR3(1.0f, -1.0f, 1.0f),
    D3DXVECTOR3(-1.0f, -1.0f, 1.0f),

    // Left face vertex positions
    D3DXVECTOR3(-1.0f, -1.0f, 1.0f),
    D3DXVECTOR3(-1.0f, 1.0f, 1.0f),
    D3DXVECTOR3(-1.0f, 1.0f, -1.0f),
    D3DXVECTOR3(-1.0f, -1.0f, -1.0f),

    // Right face vertex positions
    D3DXVECTOR3(1.0f, -1.0f, -1.0f),
    D3DXVECTOR3(1.0f, 1.0f, -1.0f),
    D3DXVECTOR3(1.0f, 1.0f, 1.0f),
    D3DXVECTOR3(1.0f, -1.0f, 1.0f)
};

WORD CubeIndeces[CUBE_INDEX_COUNT] =
{
    // Front face index data
    0, 1, 2,
    0, 2, 3,

    // Back face index data
    4, 5, 6,
    4, 6, 7,

    // Top face index data
    8, 9, 10,
    8, 10, 11,

    // Bottom face index data
    12, 13, 14,
    12, 14, 15,

    // Left face index data
    16, 17, 18,
    16, 18, 19,

    // Right face index data
    20, 21, 22,
    20, 22, 23
};

#define CUBE_SIDE_NORMAL_UP D3DXVECTOR3(0.0f, 1.0f, 0.0f)
#define CUBE_SIDE_NORMAL_DOWN D3DXVECTOR3(0.0f, -1.0f, 0.0f)
#define CUBE_SIDE_NORMAL_RIGHT D3DXVECTOR3(1.0f, 0.0f, 0.0f)
#define CUBE_SIDE_NORMAL_LEFT D3DXVECTOR3(-1.0f, 0.0f, 0.0f)
#define CUBE_SIDE_NORMAL_FRONT D3DXVECTOR3(0.0f, 0.0f, -1.0f)
#define CUBE_SIDE_NORMAL_BACK D3DXVECTOR3(0.0f, 0.0f, 1.0f)

D3DXVECTOR3 CubeSidesNormals[6] =
{
    CUBE_SIDE_NORMAL_FRONT,
    CUBE_SIDE_NORMAL_BACK,
    CUBE_SIDE_NORMAL_UP,
    CUBE_SIDE_NORMAL_DOWN,
    CUBE_SIDE_NORMAL_RIGHT,
    CUBE_SIDE_NORMAL_LEFT
};

D3DXVECTOR2 CubeSideUV[4] =
{
    D3DXVECTOR2(0.0f, 1.0f),
    D3DXVECTOR2(0.0f, 0.0f),
    D3DXVECTOR2(1.0f, 0.0f),
    D3DXVECTOR2(1.0f, 1.0f),
};
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
        g_bNeedResizeWindow = true;
        g_ScreenResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ScreenResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    }

    // Call default handler for other messages
    return DefWindowProc(hWnd, message, wParam, lParam);
}
///////////////////////////////////////////////////////////////
void CreateLogWindow()
{
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
}
///////////////////////////////////////////////////////////////
void CreateMainWindow(HINSTANCE hInstance)
{
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
        CW_USEDEFAULT,              // Position X
        CW_USEDEFAULT,              // Position Y
        g_ScreenHeight,             // Size X
        g_ScreenWidth,              // Size Y
        NULL,                       // Parent window
        NULL,                       // Menu
        WindClass.hInstance,        // Instance handle
        NULL                        // Additional application data
    );

    if (g_Window == NULL)
    {
        // If the window creation failed, exit
        MessageBox(NULL, "Error in window creating procedure", "DX9 Sample.exe", MB_OK);
        g_bNeedCloseApplication = true;
    }

    std::cout << "Window created successfully \n";

    // Show the window 
    // (You need turn subsystem to Windows (/SUBSYSTEM:WINDOWS) 
    // in VS project settings for this)
    ShowWindow(g_Window, SW_SHOW);
}
///////////////////////////////////////////////////////////////
void CreateDirect3D()
{
    std::cout << "Creating Direct3D 9 \n";
    g_Direct3D = Direct3DCreate9(D3D_SDK_VERSION);

    ZeroMemory(&g_Direct3DPresentationParams, sizeof(g_Direct3DPresentationParams));
    g_Direct3DPresentationParams.Windowed = TRUE;
    g_Direct3DPresentationParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_Direct3DPresentationParams.BackBufferFormat = D3DFMT_X8R8G8B8;
    g_Direct3DPresentationParams.EnableAutoDepthStencil = TRUE;
    g_Direct3DPresentationParams.AutoDepthStencilFormat = D3DFMT_D24X8;

    // Result value for handle function execution result
    HRESULT result = E_FAIL;

    // Create the device
    std::cout << "Creating Direct3D 9 Device \n";
    result = g_Direct3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        g_Window, // Pass the window handle here
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &g_Direct3DPresentationParams,
        &g_Direct3DDevice
    );

    if (FAILED(result))
    {
        // Handle device creation failure (add your error handling here)
        MessageBox(NULL, "Error in Direct3D 9 Device creating procedure", "DX9 Sample.exe", MB_OK);
        g_bNeedCloseApplication = true;
    }
}
///////////////////////////////////////////////////////////////
void LoadTextures()
{
    // Use D3DX to create a texture from a file based image
    // Add 
    // copy /Y "$(SolutionDir)\Samples\DirectX\DirectX 9\$(TargetName)\uv_cheker.bmp" "$(OutDir)uv_cheker.bmp"
    // To your post build events for auto copying texture to your bin folder
    HRESULT result = D3DXCreateTextureFromFile(g_Direct3DDevice, "uv_cheker.bmp", &g_Texture);

    if (FAILED(result))
    {
        MessageBox(NULL, "Could not find uv_cheker.bmp", "DX9 Sample.exe", MB_OK);
        g_bNeedCloseApplication = true;
    }
}
///////////////////////////////////////////////////////////////
void CreateGeometry()
{
    // Create and fill the vertex buffer
    std::cout << "\n";
    std::cout << "Creating vertex buffer \n";

    // Prepare vertex buffer data
    float VertexDataSize = sizeof(VERTEX_DATA);
    UINT TrianglesCount = CUBE_TRIANGLES_COUNT;
    UINT VertecesCount = TrianglesCount * 3;
    UINT VertexBufferSize = (UINT)VertexDataSize * VertecesCount;

    g_Direct3DDevice->CreateVertexBuffer(
        VertexBufferSize,           // Size of the vertex buffer
        NULL,                       // Usage
        VERTEXFVF,                  // FVF
        D3DPOOL_MANAGED,            // Memory pool
        &g_VertexBuffer,            // Pointer to the vertex buffer
        NULL                        // Handle to the resource
    );

    // Result value for handle function execution result
    HRESULT result = E_FAIL;

    // Fill the vertex buffer with data
    VERTEX_DATA* VerticesData;
    std::cout << "Locking vertex buffer \n";
    result = g_VertexBuffer->Lock(NULL, sizeof(VerticesData), (void**)&VerticesData, NULL);

    if (FAILED(result))
    {
        MessageBox(NULL, "Error in vertex buffer locking procedure", "DX9 Sample.exe", MB_OK);
        g_bNeedCloseApplication = true;
    }

    // Define a triangles
    std::cout << "Filling vertex buffer \n";

    // Iterate all vertices for define they attributes
    // Total vertices iterator is value in {0, CUBE_VERTEX_COUNT} interval, and it needs for iterate all vertices of cube
    // Side vertices iterator is value in {0; 4} interval, and it needs for get data from per side vertices data arrays
    // Side identificator is value in {0; 6} interval, and it needs for get data for all side of cube 
    for (int total_vertices_iterator = 0, side_vertices_iterator = 0, side_id = 0; total_vertices_iterator < CUBE_VERTEX_COUNT; total_vertices_iterator++, side_vertices_iterator++)
    {
        // Update support iterators
        if (side_vertices_iterator > 3)
        {
            side_vertices_iterator = 0;
            side_id++;
        }

        VerticesData[total_vertices_iterator].Position = CubeVerticesPositions[total_vertices_iterator];
        VerticesData[total_vertices_iterator].Color = { D3DCOLOR_XRGB(255, 255, 255) };
        VerticesData[total_vertices_iterator].Normal = CubeSidesNormals[side_id];
        VerticesData[total_vertices_iterator].UV = CubeSideUV[side_vertices_iterator];

        // Debug stuff
#if DEBUG_SHOW_VERTEX_COLOR == SHOW_NORMALS
        D3DXVECTOR3 VertexNormalColor = { 0, 0, 0 };

        // For debug draw normals need be transformed from {-1; 1} interval to {0, 1}
        VertexNormalColor.x = (VerticesData[total_vertices_iterator].Normal.x + 1.0f) * 0.5f;
        VertexNormalColor.y = (VerticesData[total_vertices_iterator].Normal.y + 1.0f) * 0.5f;
        VertexNormalColor.z = (VerticesData[total_vertices_iterator].Normal.z + 1.0f) * 0.5f;

        VerticesData[total_vertices_iterator].Color = { D3DCOLOR_XRGB((int)(VertexNormalColor.x * 255),
                                                                      (int)(VertexNormalColor.y * 255),
                                                                      (int)(VertexNormalColor.z * 255)) };
#elif DEBUG_SHOW_VERTEX_COLOR == SHOW_UV
        VerticesData[total_vertices_iterator].Color = { D3DCOLOR_XRGB((int)(VerticesData[total_vertices_iterator].UV.x * 255),
                                                                      (int)(VerticesData[total_vertices_iterator].UV.y * 255),
                                                                      0) };
#endif
    }

    std::cout << "Unlocking vertex buffer \n";
    result = g_VertexBuffer->Unlock();

    if (FAILED(result))
    {
        MessageBox(NULL, "Error in vertex buffer unlocking procedure", "DX9 Sample.exe", MB_OK);
        g_bNeedCloseApplication = true;
    }

    // Create and fill index buffer

    // Prepare index buffer data
    int IndecesCount = CUBE_INDEX_COUNT;
    float IndexSize = sizeof(WORD);
    UINT IndexBufferSize = IndecesCount * (UINT)IndexSize;

    std::cout << "\n";
    std::cout << "Creating index buffer \n";

    g_Direct3DDevice->CreateIndexBuffer(
        IndexBufferSize,                // Size of the index buffer
        D3DUSAGE_WRITEONLY,             // Usage
        D3DFMT_INDEX16,                 // Format
        D3DPOOL_MANAGED,                // Pool
        &g_IndexBuffer,                 // Index buffer
        0);                             // Handle to the resource

    // Fill the index buffer
    WORD* Indeces = 0;
    std::cout << "Locking index buffer \n";
    result = g_IndexBuffer->Lock(0, 0, (void**)&Indeces, 0);

    if (FAILED(result))
    {
        MessageBox(NULL, "Error in index buffer locking procedure", "DX9 Sample.exe", MB_OK);
        g_bNeedCloseApplication = true;
    }

    std::cout << "Filling index buffer \n";
    for (int iterator = 0; iterator < IndecesCount; iterator++)
    {
        Indeces[iterator] = CubeIndeces[iterator];
    }

    std::cout << "Unlocking index buffer \n";
    result = g_IndexBuffer->Unlock();

    if (FAILED(result))
    {
        MessageBox(NULL, "Error in index buffer unlocking procedure", "DX9 Sample.exe", MB_OK);
        g_bNeedCloseApplication = true;
    }
}
///////////////////////////////////////////////////////////////
void HandleDeviceLost()
{
    HRESULT result = g_Direct3DDevice->TestCooperativeLevel();

    if (result == D3DERR_DEVICELOST)
        Sleep(10);

    if (result == D3DERR_DEVICENOTRESET)
        g_bNeedReset = true;
}
///////////////////////////////////////////////////////////////
void ResetDirect3D()
{
    HRESULT result = g_Direct3DDevice->Reset(&g_Direct3DPresentationParams);

    if (result == D3DERR_INVALIDCALL)
    {
        std::cout << "Invalid call while device resetting \n";
        g_bNeedCloseApplication = true;
    }
}
///////////////////////////////////////////////////////////////
void ResizeWindow()
{
    g_Direct3DPresentationParams.BackBufferWidth = g_ScreenWidth = g_ScreenResizeWidth;
    g_Direct3DPresentationParams.BackBufferHeight = g_ScreenHeight = g_ScreenResizeHeight;

    g_ScreenResolutionAspectRatio = (float)g_ScreenWidth / (float)g_ScreenHeight;

    g_ScreenResizeWidth = g_ScreenResizeHeight = 0;
    g_bNeedReset = true;
}
///////////////////////////////////////////////////////////////
void UpdateTransformMatrices()
{
    // For our world matrix, we will just rotate the object about the y-axis.
    D3DXMATRIX matWorld;

    // Set up the rotation matrix to generate 1 full rotation (2*PI radians) 
    // every 1000 ms. To avoid the loss of precision inherent in very high 
    // floating point numbers, the system time is modulated by the rotation 
    // period before conversion to a radian angle.
    UINT iTime = timeGetTime();
    FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 2500.0f;
    D3DXMatrixRotationY(&matWorld, fAngle);
    g_Direct3DDevice->SetTransform(D3DTS_WORLD, &matWorld);

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXVECTOR3 vEyePt(0.0f, 3.0f, -5.0f);
    D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
    g_Direct3DDevice->SetTransform(D3DTS_VIEW, &matView);

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIX matProjection;
    D3DXMatrixPerspectiveFovLH(&matProjection, D3DX_PI / 4, g_ScreenResolutionAspectRatio, 1.0f, 100.0f);
    g_Direct3DDevice->SetTransform(D3DTS_PROJECTION, &matProjection);
}
///////////////////////////////////////////////////////////////
void DrawGeometry()
{
#if defined(DEBUG_SHOW_WIREFRAME)
    // Disable backface culling
    g_Direct3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // Enable FFP lighting for draw our wireframe black
    g_Direct3DDevice->SetRenderState(D3DRS_LIGHTING, true);

    // Set triangle filling from solid to wireframe
    g_Direct3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
#elif defined(DEBUG_SHOW_VERTECES)
    // Disable backface culling
    g_Direct3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // Enable FFP lighting for draw our wireframe black
    g_Direct3DDevice->SetRenderState(D3DRS_LIGHTING, true);

    // Set triangle filling from solid to point
    g_Direct3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
#else
    // Enable backface culling
    g_Direct3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    // Disable FFP lighting
    g_Direct3DDevice->SetRenderState(D3DRS_LIGHTING, false);
#endif

    // Setup our texture. Using textures introduces the texture stage states,
    // which govern how textures get blended together (in the case of multiple
    // textures) and lighting information. In this case, we are modulating
    // (blending) our texture with the diffuse color of the vertices.
#ifndef DEBUG_SHOW_VERTEX_COLOR
    g_Direct3DDevice->SetTexture(0, g_Texture);
    g_Direct3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    g_Direct3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_Direct3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g_Direct3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
#else
    // Set vertex color to output to screen
    g_Direct3DDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);

    // Set ambient lighting color 
    g_Direct3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_ARGB(255, 255, 255, 255));

    // Set vertex color to diffuse color of material
    g_Direct3DDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
#endif

    // Set the vertex buffer to be active
    g_Direct3DDevice->SetStreamSource(0, g_VertexBuffer, 0, sizeof(VERTEX_DATA));

    // Set the index buffer to be active
    g_Direct3DDevice->SetIndices(g_IndexBuffer);

    // Set the FVF
    g_Direct3DDevice->SetFVF(VERTEXFVF);

    // Draw the primitive with indeces
    g_Direct3DDevice->DrawIndexedPrimitive(
        D3DPT_TRIANGLELIST,
        NULL,
        NULL,
        CUBE_VERTEX_COUNT,
        NULL,
        CUBE_TRIANGLES_COUNT);
}
///////////////////////////////////////////////////////////////
void ClearResources()
{
    if (g_Texture)
    {
        std::cout << "Releasing texture \n";
        g_Texture->Release();
        g_Texture = nullptr;
    }

    if (g_IndexBuffer)
    {
        std::cout << "Releasing index buffer \n";
        g_IndexBuffer->Release();
        g_IndexBuffer = nullptr;
    }

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
}
///////////////////////////////////////////////////////////////
// Entry point of the application
INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, INT)
{
    // Result value for handle function execution result
    HRESULT result = E_FAIL;

    // Create log window for debug messages
    CreateLogWindow();

    // Print message into our log console
    std::cout << "Starting application\n";
    std::cout << "\n";

    // Create main window
    CreateMainWindow(hInstance);

    // Initialize Direct3D
    CreateDirect3D();

    // Creating texture for our model
    LoadTextures();

    // Create and fill vertex and index buffer
    CreateGeometry();

    // Main application loop
    std::cout << "\n";
    std::cout << "Starting event loop \n";

    MSG WindowMessage;
    ZeroMemory(&WindowMessage, sizeof(WindowMessage));
    while (WindowMessage.message != WM_QUIT && !g_bNeedCloseApplication)
    {
        if (PeekMessage(&WindowMessage, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&WindowMessage);
            DispatchMessage(&WindowMessage);
        }

        UpdateTransformMatrices();

        // Clear the back buffer
        g_Direct3DDevice->Clear(NULL, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, NULL);

        // Begin the scene
        g_Direct3DDevice->BeginScene();

        // Draw our indexed cube primitive and set textures and render states
        DrawGeometry();

        // End the scene
        g_Direct3DDevice->EndScene();

        // Present frame to screen
        result = g_Direct3DDevice->Present(NULL, NULL, NULL, NULL);

        // Device lost event handling
        if (result == D3DERR_DEVICELOST)
        {
            std::cout << "Device was been lost - handle \n";
            HandleDeviceLost();
        }

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_bNeedResizeWindow)
        {
            ResizeWindow();

            g_bNeedResizeWindow = false;
        }

        // Device reseting code
        if (g_bNeedReset)
        {
            std::cout << "Resetting Direct3D Device \n";

            ResetDirect3D();

            g_bNeedReset = false;
        }
    }

    //-------------------DESTROYING CODE-------------------//
    std::cout << "\n";
    std::cout << "Ending event loop \n";
    std::cout << "\n";

    std::cout << "Releasing resources \n";
    ClearResources();

    std::cout << "Destroying window \n";
    DestroyWindow(g_Window);

    std::cout << "\n";
    std::cout << "Application closed successfully, closing window after 3 seconds \n";
    Sleep(3000);

    return 0;
}
///////////////////////////////////////////////////////////////
