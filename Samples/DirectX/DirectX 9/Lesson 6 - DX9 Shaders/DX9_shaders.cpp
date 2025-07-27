///////////////////////////////////////////////////////////////
// Date: 22.07.2025
// Author: NS_Deathman
// Deathman's samples mystery box
///////////////////////////////////////////////////////////////
// Lesson ¹6 - "Shaders"
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
D3DPRESENT_PARAMETERS g_Direct3DPresentationParams = {};
///////////////////////////////////////////////////////////////
HWND g_Window = nullptr;
///////////////////////////////////////////////////////////////
IDirect3DVertexShader9* g_VertexShader;
ID3DXConstantTable* g_VertexShaderConstantTable;
IDirect3DPixelShader9* g_PixelShader;
ID3DXConstantTable* g_PixelShaderConstantTable;
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

LPDIRECT3DVERTEXDECLARATION9 g_VertexDeclaration = NULL;

D3DVERTEXELEMENT9 Declaration[] =
{
    { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
    { 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
    { 0, 36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END()
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

#define CUBE_SIDE_NORMAL_FRONT D3DXVECTOR3(0.0f, 0.0f, -1.0f)
#define CUBE_SIDE_NORMAL_BACK D3DXVECTOR3(0.0f, 0.0f, 1.0f)
#define CUBE_SIDE_NORMAL_UP D3DXVECTOR3(0.0f, 1.0f, 0.0f)
#define CUBE_SIDE_NORMAL_DOWN D3DXVECTOR3(0.0f, -1.0f, 0.0f)
#define CUBE_SIDE_NORMAL_RIGHT D3DXVECTOR3(-1.0f, 0.0f, 0.0f)
#define CUBE_SIDE_NORMAL_LEFT D3DXVECTOR3(1.0f, 0.0f, 0.0f)

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
#define SAFE_RELEASE(x)		\
	{						\
		if (x)				\
		{					\
			(x)->Release();	\
			(x) = NULL;		\
		}					\
	}

void MakeErrorMessage(LPCSTR Message)
{
    MessageBox(NULL, Message, "DX9 Sample.exe", MB_OK);
    g_bNeedCloseApplication = true;
}

void Assert(bool Condition, LPCSTR Message)
{
    if(Condition)
        MakeErrorMessage(Message);
}
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
    // (Ensure to configure project settings correctly to avoid issues with 
    // worldwide and Unicode symbols in Visual Studio)
    LPCSTR window_class_name = (LPCSTR)"DirectXSampleWindowClass"; // Define a name for the window class.
    LPCSTR window_name = (LPCSTR)"DirectX 9 Sample";               // Define a title for the window.

    // Initialize a WNDCLASS structure to define the window
    WNDCLASS WindClass = {}; // Use default initialization to zero out the struct.
    WindClass.lpfnWndProc = WindowProcedure;         // Specify the function to handle window messages.
    WindClass.hInstance = hInstance;                 // Store the application instance handle.
    WindClass.lpszClassName = window_class_name;     // Set the name of the window class.
    WindClass.hCursor = LoadCursor(NULL, IDC_ARROW); // Load the default arrow cursor.

    // Register the window class with the operating system
    std::cout << "Window class registering \n";       // Output message indicating registration process.
    RegisterClass(&WindClass);                        // Call Windows API to register the defined window class.

    // Create the main application window
    g_Window = CreateWindowEx(
        NULL,                                          // Optional window styles (default)
        window_class_name,                             // Name of the registered window class
        window_name,                                   // Title text displayed in the window
        WS_OVERLAPPEDWINDOW,                           // Window style (overlapped window with borders)
        CW_USEDEFAULT,                                 // Default x position on screen
        CW_USEDEFAULT,                                 // Default y position on screen
        g_ScreenHeight,                                // Width of the window
        g_ScreenWidth,                                 // Height of the window
        NULL,                                          // Handle to parent window (no parent)
        NULL,                                          // Handle to menu (no menu)
        WindClass.hInstance,                           // Instance handle from the registered class
        NULL);                                         // Additional application data (none)

    // Check if the window was created successfully
    if (g_Window == NULL)
    {
        // If the window creation failed, show an error message and set a flag to close the application
        MessageBox(NULL, "Error in window creating procedure", "DX9 Sample.exe", MB_OK);
        g_bNeedCloseApplication = true; // Flag to signal that the application should close.
    }

    std::cout << "Window created successfully \n"; // Output message indicating successful window creation.

    // Show the newly created window
    // (Make sure to set the subsystem to Windows (/SUBSYSTEM:WINDOWS) in Visual Studio project settings)
    ShowWindow(g_Window, SW_SHOW); // Display the window with the specified command to show it.
}
///////////////////////////////////////////////////////////////
void CreateDirect3D()
{
    std::cout << "Creating Direct3D 9 \n"; // Output a message indicating that Direct3D 9 is being created.
    
    // Initialize Direct3D COM object
    g_Direct3D = Direct3DCreate9(D3D_SDK_VERSION); // Create a Direct3D object based on the SDK version specified.

    // Zero out the structure to ensure all fields are initialized to zero
    ZeroMemory(&g_Direct3DPresentationParams, sizeof(g_Direct3DPresentationParams));
    
    // Set presentation parameters for the Direct3D device
    g_Direct3DPresentationParams.Windowed = TRUE; // Run in windowed mode (not fullscreen).
    g_Direct3DPresentationParams.SwapEffect = D3DSWAPEFFECT_DISCARD; // Indicates how to handle buffers when presenting.
    g_Direct3DPresentationParams.BackBufferFormat = D3DFMT_X8R8G8B8; // Set the back buffer format to 32-bit RGB (with no alpha).
    g_Direct3DPresentationParams.EnableAutoDepthStencil = TRUE; // Enable automatic depth buffering.
    g_Direct3DPresentationParams.AutoDepthStencilFormat = D3DFMT_D24X8; // Set the depth buffer format to a 24-bit depth and 8 bits for stencil.

    // Result value for handling function execution result
    HRESULT result = E_FAIL; // Initialize 'result' to E_FAIL which indicates failure by default.

    // Create the Direct3D device
    std::cout << "Creating Direct3D 9 Device \n"; // Output a message indicating that the Direct3D device is being created.
    
    // Call CreateDevice to create a new Direct3D device
    result = g_Direct3D->CreateDevice( D3DADAPTER_DEFAULT,                  // Use the default adapter (usually the primary display).
                                       D3DDEVTYPE_HAL,                      // Use HAL (Hardware Abstraction Layer) device.
                                       g_Window,                            // Pass the window handle where the device will render.
                                       D3DCREATE_HARDWARE_VERTEXPROCESSING, // Specify hardware vertex processing for better performance.
                                       &g_Direct3DPresentationParams,       // Provide the previously configured presentation parameters.
                                       &g_Direct3DDevice );                 // Output the created device to this variable.

    // Handle device creation failure
    Assert(FAILED(result), "Error in Direct3D 9 Device creating procedure"); // Assert if device creation failed, outputting an error message.
}
///////////////////////////////////////////////////////////////
void LoadTextures()
{
    // Use D3DX to create a texture from a file based image
    // Add 
    // copy /Y "$(SolutionDir)\Samples\DirectX\DirectX 9\$(TargetName)\uv_cheker.bmp" "$(OutDir)uv_cheker.bmp"
    // To your post build events for auto copying texture to your bin folder
    HRESULT result = D3DXCreateTextureFromFile(g_Direct3DDevice, "uv_cheker.bmp", &g_Texture);

    Assert(FAILED(result), "Could not find uv_cheker.bmp");
}
///////////////////////////////////////////////////////////////
void CompileShaders()
{
    // Result value for handle function execution result
    HRESULT result = E_FAIL; // Initialize 'result' to E_FAIL, indicative of failure.

    ID3DXBuffer* ErrorBuffer = NULL; // Buffer to hold any compilation errors.

    // Vertex shader
    ID3DXBuffer* VertexShaderBuffer = NULL; // Buffer to hold compiled vertex shader.
    std::string ShaderFilePath = (std::string)"shader.txt"; // Path to the shader source file.
    std::cout << "Compiling vertex shader \n"; // Output message indicating the start of vertex shader compilation.
    
    // Compile the vertex shader from file
    result = D3DXCompileShaderFromFile( ShaderFilePath.c_str(),         // Path to the shader source
                                        nullptr,                        // No defines needed
                                        nullptr,                        // No include handler needed
                                        "VSMain",                       // Entry point for the vertex shader
                                        "vs_3_0",                       // Shader model (version)
                                        NULL,                           // No flags specified
                                        &VertexShaderBuffer,            // Output buffer for compiled shader
                                        &ErrorBuffer,                   // Output buffer for any errors
                                        &g_VertexShaderConstantTable ); // Constant table for the vertex shader

    // Check for compilation errors in the vertex shader
    if (ErrorBuffer)
    {
        std::cout << "Vertex shader error \n"; // Indicate an error occurred during compilation.
        std::cout << (char*)ErrorBuffer->GetBufferPointer(); // Output the error message.
        std::cout << "\n"; // New line for better readability.
    }

    // If the compilation failed, handle the error
    if (FAILED(result))
    {
        MakeErrorMessage((char*)ErrorBuffer->GetBufferPointer()); // Show a detailed error message.
        ErrorBuffer->Release(); // Release the error buffer to avoid memory leaks.
    }

    // Create the vertex shader object from the compiled buffer
    g_Direct3DDevice->CreateVertexShader((DWORD*)VertexShaderBuffer->GetBufferPointer(), &g_VertexShader);

    // Pixel shader
    ID3DXBuffer* PixelShaderBuffer = NULL; // Buffer to hold compiled pixel shader.
    std::cout << "Compiling pixel shader \n"; // Output message indicating the start of pixel shader compilation.
    
    // Compile the pixel shader from file
    result = D3DXCompileShaderFromFile( ShaderFilePath.c_str(),         // Path to the same shader source
                                        nullptr,                        // No defines needed
                                        nullptr,                        // No include handler needed
                                        "PSMain",                       // Entry point for the pixel shader
                                        "ps_3_0",                       // Shader model (version)
                                        NULL,                           // No flags specified
                                        &PixelShaderBuffer,             // Output buffer for compiled shader
                                        &ErrorBuffer,                   // Output buffer for any errors
                                        &g_PixelShaderConstantTable );  // Constant table for the pixel shader

    // Check for compilation errors in the pixel shader
    if (ErrorBuffer)
    {
        std::cout << "Pixel shader error \n"; // Indicate an error occurred during compilation.
        std::cout << (char*)ErrorBuffer->GetBufferPointer(); // Output the error message.
        std::cout << "\n"; // New line for better readability.
    }

    // If the compilation failed, handle the error
    if (FAILED(result))
    {
        MakeErrorMessage((char*)ErrorBuffer->GetBufferPointer()); // Show a detailed error message.
        ErrorBuffer->Release(); // Release the error buffer to avoid memory leaks.
    }

    // Create the pixel shader object from the compiled buffer
    g_Direct3DDevice->CreatePixelShader((DWORD*)PixelShaderBuffer->GetBufferPointer(), &g_PixelShader);
}
///////////////////////////////////////////////////////////////
void CreateGeometry()
{
    // Create a vertex declaration which describes the vertex format used in the vertex buffer
    g_Direct3DDevice->CreateVertexDeclaration(Declaration, &g_VertexDeclaration);

    // Log creation of the vertex buffer
    std::cout << "\n";
    std::cout << "Creating vertex buffer \n";

    // Prepare vertex buffer data
    float VertexDataSize = sizeof(VERTEX_DATA); // Size of a single vertex structure
    UINT TrianglesCount = CUBE_TRIANGLES_COUNT;  // Total number of triangles in the cube
    UINT VertecesCount = TrianglesCount * 3;     // Each triangle has 3 vertices
    UINT VertexBufferSize = (UINT)VertexDataSize * VertecesCount; // Total size required for the vertex buffer

    // Create the vertex buffer in managed memory pool with specified parameters
    g_Direct3DDevice->CreateVertexBuffer( VertexBufferSize, // Size of the vertex buffer
                                          NULL,             // Usage flag (default)
                                          VERTEXFVF,        // Flexible Vertex Format (defines the layout of the vertex data)
                                          D3DPOOL_MANAGED,  // Memory pool type (managed by Direct3D)
                                          &g_VertexBuffer,  // Pointer to the created vertex buffer
                                          NULL );           // Handle to the resource (not needed here)

    // Result variable to check success/failure of operations
    HRESULT result = E_FAIL;

    // Fill the vertex buffer with data
    VERTEX_DATA* VerticesData; // Pointer to hold vertex data during locking

    std::cout << "Locking vertex buffer \n"; // Indicating that we're about to fill the buffer

    // Lock the vertex buffer to gain access to write data into it
    result = g_VertexBuffer->Lock(NULL, sizeof(VerticesData), (void**)&VerticesData, NULL);

    // Check if locking was successful; if not, assert and show an error message
    Assert(FAILED(result), "Error in vertex buffer locking procedure");

    // Log filling process of vertex buffer
    std::cout << "Filling vertex buffer \n";

    // Iterate through all vertices to define their attributes
    // Total vertices iterator ranges over the total number of vertices defined for the cube
    // Side vertices iterator helps track vertices corresponding to each side of the cube
    // Side identifier keeps track of which side of the cube is being processed
    for (int total_vertices_iterator = 0, side_vertices_iterator = 0, side_id = 0; 
         total_vertices_iterator < CUBE_VERTEX_COUNT; 
         total_vertices_iterator++, side_vertices_iterator++)
    {
        // Reset side vertices iterator and increment side_id after processing 4 vertices per side
        if (side_vertices_iterator > 3)
        {
            side_vertices_iterator = 0; // Reset to first vertex on next iteration
            side_id++; // Move to the next side of the cube
        }

        // Assign position, color, normal, and UV mapping to the current vertex
        VerticesData[total_vertices_iterator].Position = CubeVerticesPositions[total_vertices_iterator];
        VerticesData[total_vertices_iterator].Color = { D3DCOLOR_XRGB(255, 255, 255) }; // Set color to white
        VerticesData[total_vertices_iterator].Normal = CubeSidesNormals[side_id]; // Get the normal for the current side
        VerticesData[total_vertices_iterator].UV = CubeSideUV[side_vertices_iterator]; // Assign UV coordinates
    }

    // Unlock the vertex buffer after filling it with data
    std::cout << "Unlocking vertex buffer \n";
    result = g_VertexBuffer->Unlock(); // Unlocking the buffer allows GPU to access the data

    // Check if unlocking was successful; if not, assert and show an error message
    Assert(FAILED(result), "Error in vertex buffer unlocking procedure");

    // Create and fill index buffer
    std::cout << "\n";
    std::cout << "Creating index buffer \n";

    // Prepare index buffer data
    int IndecesCount = CUBE_INDEX_COUNT; // Total number of indices required
    float IndexSize = sizeof(WORD); // Size of an individual index (16 bits)
    UINT IndexBufferSize = IndecesCount * (UINT)IndexSize; // Calculate total size of the index buffer

    // Create the index buffer with specified parameters
    g_Direct3DDevice->CreateIndexBuffer( IndexBufferSize,       // Size of the index buffer
                                         D3DUSAGE_WRITEONLY,    // Buffer usage (write-only)
                                         D3DFMT_INDEX16,        // Format of the indices (16-bit)
                                         D3DPOOL_MANAGED,       // Memory access
                                         &g_IndexBuffer,        // Index buffer
                                         0 );                   // Handle to the resource

    // Fill the index buffer
    WORD* Indeces = 0;
    std::cout << "Locking index buffer \n";
    result = g_IndexBuffer->Lock(0, 0, (void**)&Indeces, 0);

    Assert(FAILED(result), "Error in index buffer locking procedure");

    std::cout << "Filling index buffer \n";
    for (int iterator = 0; iterator < IndecesCount; iterator++)
    {
        Indeces[iterator] = CubeIndeces[iterator];
    }

    std::cout << "Unlocking index buffer \n";
    result = g_IndexBuffer->Unlock();

    Assert(FAILED(result), "Error in index buffer unlocking procedure");
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
    HRESULT result = E_FAIL;

    result = g_Direct3DDevice->Reset(&g_Direct3DPresentationParams);

    Assert(result == D3DERR_INVALIDCALL, "Invalid call while device resetting");
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
    //-- World Matrix --//
    // The World Matrix defines the position, rotation, and scale of an object in the world space. 
    // It transforms local object coordinates (object space) into world coordinates.
    //      Translation: Moves the object to its desired location.
    //      Rotation: Affects the orientation of the object.
    //      Scaling: Changes the size of the object.
    D3DXMATRIX matWorld;

    // For our world matrix, we will just rotate the object about the y-axis.
    // Set up the rotation matrix to generate 1 full rotation (2*PI radians) 
    // every 1000 ms. To avoid the loss of precision inherent in very high 
    // floating point numbers, the system time is modulated by the rotation 
    // period before conversion to a radian angle.
    UINT iTime = timeGetTime();
    FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 2500.0f;
    D3DXMatrixRotationY(&matWorld, fAngle);
    g_VertexShaderConstantTable->SetMatrix(g_Direct3DDevice, "matWorld", &matWorld);

    //-- View Matrix --//
    // The View Matrix defines the camera's position and orientation in the world. 
    // It essentially represents the transformation needed to convert world coordinates into view coordinates (camera space).
    //      Eye Position : Where the camera is located in the world.
    //      Look - at Point : The point in the world that the camera is looking at.
    //      Up Vector : Defines which direction is "up" for the camera, helping to create the proper orientation.

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXVECTOR3 vEyePt(0.0f, 3.0f, -5.0f);
    D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
    g_VertexShaderConstantTable->SetMatrix(g_Direct3DDevice, "matView", &matView);

    //-- Projection Matrix --//
    // Although not a part of the WorldView matrix, 
    // it's essential to understand it since it’s combined with the WorldView matrix 
    // to form the WorldViewProjection matrix. 
    // The Projection Matrix transforms coordinates from view space to normalized device coordinates (NDC), 
    // preparing them for perspective division and viewport transformation.

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIX matProjection;
    float Fov = D3DX_PI / 4;
    float ZNear = 1.0f;
    float ZFar = 100.0f;
    D3DXMatrixPerspectiveFovLH(&matProjection, Fov, g_ScreenResolutionAspectRatio, ZNear, ZFar);
    g_VertexShaderConstantTable->SetMatrix(g_Direct3DDevice, "matProjection", &matProjection);

    //-- WorldView Matrix --//
    // The WorldView Matrix combines the World and View matrices.
    // It transforms coordinates from object space to view space.
    // This matrix is formed by multiplying the World matrix by the View matrix :
    D3DXMATRIX matWorldView;
    matWorldView = matWorld * matView;
    g_VertexShaderConstantTable->SetMatrix(g_Direct3DDevice, "matWorldView", &matWorldView);

    //-- WorldViewProjection Matrix --//
    // The WorldViewProjection(WVP) Matrix is the combination of the World, View, and Projection matrices.
    // It transforms coordinates from object space directly to clip space in a single operation :
    D3DXMATRIX matWorldViewProjection;
    matWorldViewProjection = matWorld * matView * matProjection;
    g_VertexShaderConstantTable->SetMatrix(g_Direct3DDevice, "matWorldViewProjection", &matWorldViewProjection);
}
///////////////////////////////////////////////////////////////
void DrawGeometry()
{
    // Enable backface culling to prevent rendering of faces that are facing away from the camera,
    // improving performance by reducing the number of triangles processed.
    g_Direct3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); // Counter-clockwise face culling

    // Setup our texture. Bind the texture resource to the shader's texture register (S0).
    // This allows the pixel shader to access and use this texture when rendering the geometry.
    g_Direct3DDevice->SetTexture(0, g_Texture); // Texture stage 0 (S0) is being set with the texture resource

    // Set the vertex buffer as the active source for vertex data. 
    // The vertex buffer will be read starting at offset 0, and each vertex is of size sizeof(VERTEX_DATA).
    g_Direct3DDevice->SetStreamSource(0, g_VertexBuffer, 0, sizeof(VERTEX_DATA));

    // Set the index buffer as the source for indices that define which vertices to draw.
    // This allows indexed drawing, which is more efficient than drawing each vertex individually.
    g_Direct3DDevice->SetIndices(g_IndexBuffer);

    // Specify the Flexible Vertex Format (FVF) to inform the graphics pipeline about vertex structure/layout.
    // This format includes information such as position, color, normal, and texture coordinates of each vertex.
    g_Direct3DDevice->SetFVF(VERTEXFVF);

    // Set the vertex shader to be used for processing vertex data.
    // The vertex shader will transform vertices and perform any necessary manipulations before rasterization.
    g_Direct3DDevice->SetVertexShader(g_VertexShader);

    // Set the pixel shader that will process the pixels of the rendered output.
    // This shader determines how the final colors of the pixels are computed using texture and lighting.
    g_Direct3DDevice->SetPixelShader(g_PixelShader);

    // Draw the primitive using indexed drawing. Here we specify:
    // - The type of primitive to render (D3DPT_TRIANGLELIST means each set of three indices defines a triangle).
    // - NULL parameters for the base vertex index (not used), base vertex for offset calculation (not used),
    //   total vertex count (CUBE_VERTEX_COUNT), and start index in the index buffer (NULL for automatic handling).
    // - The number of triangles to draw (CUBE_TRIANGLES_COUNT), which corresponds to the number of triangles defined.
    g_Direct3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,     // Primitive type: triangle list
                                            NULL,                   // Base vertex index (not needed for indexed drawing)
                                            NULL,                   // Min index (used for internal calculations, NULL for auto)
                                            CUBE_VERTEX_COUNT,      // Total number of vertices to consider from the vertex buffer
                                            NULL,                   // Start index in the index buffer (NULL defaults to zero)
                                            CUBE_TRIANGLES_COUNT ); // Number of triangles to draw
}
///////////////////////////////////////////////////////////////
void ClearResources()
{
    SAFE_RELEASE(g_VertexShader);

    SAFE_RELEASE(g_PixelShader);

    SAFE_RELEASE(g_Texture);

    SAFE_RELEASE(g_IndexBuffer);

    SAFE_RELEASE(g_VertexBuffer);

    SAFE_RELEASE(g_Direct3DDevice);

    SAFE_RELEASE(g_Direct3D);
}
///////////////////////////////////////////////////////////////
// Entry point of the application
INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, INT)
{
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

    // Compile shaders
    CompileShaders();

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

        // Result value for handle function execution result
        HRESULT result = E_FAIL;

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
