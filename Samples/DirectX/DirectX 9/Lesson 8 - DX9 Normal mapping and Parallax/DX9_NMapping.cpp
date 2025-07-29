///////////////////////////////////////////////////////////////
// Date: 22.07.2025
// Author: NS_Deathman
// Deathman's samples mystery box
///////////////////////////////////////////////////////////////
// Lesson ¹7 - "OBJ Mesh loading"
///////////////////////////////////////////////////////////////
#include "stdafx.h" 
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
#include "MeshLoader.h"
///////////////////////////////////////////////////////////////
CMeshLoader g_MeshLoader;
///////////////////////////////////////////////////////////////
#include "DXUT_timer.h"
///////////////////////////////////////////////////////////////
CDXUTTimer g_Timer;
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

    Print("Console log window created successfully");
    Print("\n");
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
    Print("Window class registering");                // Output message indicating registration process.
    RegisterClass(&WindClass);                        // Call Windows API to register the defined window class.

    // Create the main application window
    g_Window = CreateWindowEx( NULL,                  // Optional window styles (default)
                               window_class_name,     // Name of the registered window class
                               window_name,           // Title text displayed in the window
                               WS_OVERLAPPEDWINDOW,   // Window style (overlapped window with borders)
                               CW_USEDEFAULT,         // Default x position on screen
                               CW_USEDEFAULT,         // Default y position on screen
                               g_ScreenHeight,        // Width of the window
                               g_ScreenWidth,         // Height of the window
                               NULL,                  // Handle to parent window (no parent)
                               NULL,                  // Handle to menu (no menu)
                               WindClass.hInstance,   // Instance handle from the registered class
                               NULL );                // Additional application data (none)

    // Check if the window was created successfully
    if (g_Window == NULL)
    {
        // If the window creation failed, show an error message and set a flag to close the application
        MessageBox(NULL, "Error in window creating procedure", "DX9 Sample.exe", MB_OK);
        g_bNeedCloseApplication = true; // Flag to signal that the application should close.
    }

    Print("Window created successfully"); // Output message indicating successful window creation.

    // Show the newly created window
    // (Make sure to set the subsystem to Windows (/SUBSYSTEM:WINDOWS) in Visual Studio project settings)
    ShowWindow(g_Window, SW_SHOW); // Display the window with the specified command to show it.
}
///////////////////////////////////////////////////////////////
void CreateDirect3D()
{
    Print("Creating Direct3D 9"); // Output a message indicating that Direct3D 9 is being created.
    
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
    Print("Creating Direct3D 9 Device"); // Output a message indicating that the Direct3D device is being created.
    
    // Call CreateDevice to create a new Direct3D device
    result = g_Direct3D->CreateDevice( D3DADAPTER_DEFAULT,                  // Use the default adapter (usually the primary display).
                                       D3DDEVTYPE_HAL,                      // Use HAL (Hardware Abstraction Layer) device.
                                       g_Window,                            // Pass the window handle where the device will render.
                                       D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, // Specify hardware vertex processing for better performance.
                                       &g_Direct3DPresentationParams,       // Provide the previously configured presentation parameters.
                                       &g_Direct3DDevice );                 // Output the created device to this variable.

    // Handle device creation failure
    ASSERT(SUCCEEDED(result), "Error in Direct3D 9 Device creating procedure"); // ASSERT if device creation failed, outputting an error message.
}
///////////////////////////////////////////////////////////////
void LoadMeshes()
{
    // Use microsoft mesh loader class to create a texture from a file based image
    // Add 
    // copy /Y "$(SolutionDir)\Samples\DirectX\DirectX 9\$(TargetName)\mesh.obj" "$(OutDir)mesh.obj"
    // and
    // copy /Y "$(SolutionDir)\Samples\DirectX\DirectX 9\$(TargetName)\mesh.mtl" "$(OutDir)mesh.mtl"
    // To your post build events for auto copying texture to your bin folder
    g_MeshLoader.Create(g_Direct3DDevice, "", "mesh.obj");
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
    Print("Compiling vertex shader"); // Output message indicating the start of vertex shader compilation.
    
    // Compile the vertex shader from file
    result = D3DXCompileShaderFromFile( ShaderFilePath.c_str(),         // Path to the shader source
                                        nullptr,                        // No defines needed
                                        nullptr,                        // No include handler needed
                                        "VSMain",                       // Entry point for the vertex shader
                                        "vs_3_0",                       // Shader model (version)
                                        D3DXSHADER_OPTIMIZATION_LEVEL3,
                                        &VertexShaderBuffer,            // Output buffer for compiled shader
                                        &ErrorBuffer,                   // Output buffer for any errors
                                        &g_VertexShaderConstantTable ); // Constant table for the vertex shader

    // Check for compilation errors in the vertex shader
    if (ErrorBuffer)
    {
        Print("Vertex shader error"); // Indicate an error occurred during compilation.
        Print((char*)ErrorBuffer->GetBufferPointer()); // Output the error message.
        Print("\n"); // New line for better readability.
    }

    // If the compilation failed, handle the error
    if (FAILED(result))
    {
        ERROR_MESSAGE((char*)ErrorBuffer->GetBufferPointer()); // Show a detailed error message.
        ErrorBuffer->Release(); // Release the error buffer to avoid memory leaks.
        g_bNeedCloseApplication = true;
    }
    else
    {
        // Create the vertex shader object from the compiled buffer
        g_Direct3DDevice->CreateVertexShader((DWORD*)VertexShaderBuffer->GetBufferPointer(), &g_VertexShader);
    }

    // Pixel shader
    ID3DXBuffer* PixelShaderBuffer = NULL; // Buffer to hold compiled pixel shader.
    Print("Compiling pixel shader"); // Output message indicating the start of pixel shader compilation.
    
    // Compile the pixel shader from file
    result = D3DXCompileShaderFromFile( ShaderFilePath.c_str(),         // Path to the same shader source
                                        nullptr,                        // No defines needed
                                        nullptr,                        // No include handler needed
                                        "PSMain",                       // Entry point for the pixel shader
                                        "ps_3_0",                       // Shader model (version)
                                        D3DXSHADER_OPTIMIZATION_LEVEL3,
                                        &PixelShaderBuffer,             // Output buffer for compiled shader
                                        &ErrorBuffer,                   // Output buffer for any errors
                                        &g_PixelShaderConstantTable );  // Constant table for the pixel shader

    // Check for compilation errors in the pixel shader
    if (ErrorBuffer)
    {
        Print("Pixel shader error"); // Indicate an error occurred during compilation.
        Print((char*)ErrorBuffer->GetBufferPointer()); // Output the error message.
        Print("\n"); // New line for better readability.
    }

    // If the compilation failed, handle the error
    if (FAILED(result))
    {
        ERROR_MESSAGE((char*)ErrorBuffer->GetBufferPointer()); // Show a detailed error message.
        ErrorBuffer->Release(); // Release the error buffer to avoid memory leaks.
        g_bNeedCloseApplication = true;
    }
    else
    {
        // Create the pixel shader object from the compiled buffer
        g_Direct3DDevice->CreatePixelShader((DWORD*)PixelShaderBuffer->GetBufferPointer(), &g_PixelShader);
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
    HRESULT result = E_FAIL;

    result = g_Direct3DDevice->Reset(&g_Direct3DPresentationParams);

    ASSERT(result != D3DERR_INVALIDCALL, "Invalid call while device resetting");
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
void CalculateLightPosition()
{
    float iTime = (float)g_Timer.GetTime() * 0.5f;
    D3DXVECTOR4 LightPosition = D3DXVECTOR4(cos(iTime), 0.0f, sin(iTime), 0.0f) * 10.0f;
    g_PixelShaderConstantTable->SetVector(g_Direct3DDevice, "LightPosition", &LightPosition);
}

void SetPixelShaderConstants()
{
    D3DXVECTOR4 ScreenResolution = D3DXVECTOR4((float)g_ScreenWidth, (float)g_ScreenHeight, 1.0f / g_ScreenWidth, 1.0f / g_ScreenHeight);
    g_PixelShaderConstantTable->SetVector(g_Direct3DDevice, "ScreenResolution", &ScreenResolution);
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

    float iTime = (float)g_Timer.GetTime();
    FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 20.0f;
    D3DXMatrixRotationY(&matWorld, fAngle);
    g_VertexShaderConstantTable->SetMatrix(g_Direct3DDevice, "matWorld", &matWorld);

    //-- View Matrix --//
    // The View Matrix defines the camera's position and orientation in the world. 
    // It essentially represents the transformation needed to convert world coordinates into view coordinates (camera space).
    //      Eye Position : Where the camera is located in the world.
    //      Look - at Point : The point in the world that the camera is looking at.
    //      Up Vector : Defines which direction is "up" for the camera, helping to create the proper orientation.

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. 
    D3DXVECTOR3 vEyePt(0.0f, 0.0f, -5.0f);
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
    // a perpsective transform, we need the field of view,
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIX matProjection;
    float Fov = D3DXToRadian(25.0f);
    float ZNear = 0.01f;
    float ZFar = 100.0f;
    D3DXMatrixPerspectiveFovLH(&matProjection, Fov, g_ScreenResolutionAspectRatio, ZNear, ZFar);
    g_VertexShaderConstantTable->SetMatrix(g_Direct3DDevice, "matProjection", &matProjection);

    //-- WorldView Matrix --//
    // The WorldView Matrix combines the World and View matrices.
    // It transforms coordinates from object space to view space.
    // This matrix is formed by multiplying the World matrix by the View matrix :
    D3DXMATRIX matWorldView;
    matWorldView = matWorld * matView;

    // On the CPU, 2D arrays are generally stored in row-major ordering, so the order in memory goes x[0][0], x[0][1], ... 
    // In HLSL, matrix declarations default to column-major ordering, so the order goes x[0][0], x[1][0], ...
    // https://stackoverflow.com/questions/32037617/why-is-this-transpose-required-in-my-worldviewproj-matrix
    D3DXMATRIX matWorldViewTransposed;
    D3DXMatrixTranspose(&matWorldViewTransposed, &matWorldView);
    g_VertexShaderConstantTable->SetMatrix(g_Direct3DDevice, "matWorldView", &matWorldViewTransposed);

    //-- WorldViewProjection Matrix --//
    // The WorldViewProjection(WVP) Matrix is the combination of the World, View, and Projection matrices.
    // It transforms coordinates from object space directly to clip space in a single operation :
    D3DXMATRIX matWorldViewProjection;
    matWorldViewProjection = matWorldView * matProjection;
    g_VertexShaderConstantTable->SetMatrix(g_Direct3DDevice, "matWorldViewProjection", &matWorldViewProjection);
}
///////////////////////////////////////////////////////////////
void SetTextureFiltration(DWORD Stage)
{
    // Set texture sampler states to control texture filtering behavior.
    g_Direct3DDevice->SetSamplerState(Stage, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC); // Use anisotropic filtering for magnification.
    g_Direct3DDevice->SetSamplerState(Stage, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC); // Use linear filtering for minification.
    g_Direct3DDevice->SetSamplerState(Stage, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR); // Use linear filtering for mipmap levels.

    // Set the maximum level of anisotropy for texture sampling.
    g_Direct3DDevice->SetSamplerState(Stage, D3DSAMP_MAXANISOTROPY, 16); // Max anisotropy level set to 16.
}
///////////////////////////////////////////////////////////////
void DrawGeometry()
{
    // Enable backface culling to prevent rendering of faces that are facing away from the camera.
    // This improves performance by reducing the number of triangles processed by the GPU.
    g_Direct3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // Set culling mode to none (i.e., render all faces).

    // Set the vertex shader to be used for processing vertex data.
    // The vertex shader will transform vertices and perform any necessary manipulations before the rasterization stage.
    g_Direct3DDevice->SetVertexShader(g_VertexShader);

    // Set the pixel shader that will process the pixels of the rendered output.
    // This shader determines how the final colors of the pixels are computed using texture and lighting information.
    g_Direct3DDevice->SetPixelShader(g_PixelShader);

    // Loop through each material subset in the mesh. Each subset corresponds to a different material.
    for (UINT iSubset = 0; iSubset < g_MeshLoader.GetNumMaterials(); iSubset++)
    {
        // Retrieve the mesh object from the mesh loader.
        ID3DXMesh* pMesh = g_MeshLoader.GetMesh();
        if (pMesh) // Check if the mesh was successfully retrieved.
        {
            // Get the material associated with the current subset index.
            Material* pMaterial = g_MeshLoader.GetMaterial(iSubset);

            // Setup our albedo texture. Bind the texture resource to the shader's texture register (S0).
            // This allows the pixel shader to access and use this texture when rendering the geometry.
            g_Direct3DDevice->SetTexture(0, pMaterial->pTextureAlbedo); // Bind the albedo texture to register s0.

            // Setup filtration types for texture, binded to s0 register
            SetTextureFiltration(0);

            // Setup our bump texture. Bind the texture resource to the shader's texture register (S1).
            g_Direct3DDevice->SetTexture(1, pMaterial->pTextureBump); // Bind the bump texture to register s1.

            // Setup filtration types for texture, binded to s1 register
            SetTextureFiltration(1);

            // Setup our height texture. Bind the texture resource to the shader's texture register (S2).
            g_Direct3DDevice->SetTexture(2, pMaterial->pTextureHeight); // Bind the bump texture to register s2.

            // Setup filtration types for texture, binded to s2 register
            SetTextureFiltration(2);

            // Draw the current subset of the mesh using the appropriate material and textures.
            pMesh->DrawSubset(iSubset);
        }
    }
}
///////////////////////////////////////////////////////////////
void ClearResources()
{
    SAFE_RELEASE(g_VertexShader);

    SAFE_RELEASE(g_PixelShader);

    g_MeshLoader.Destroy();

    SAFE_RELEASE(g_Direct3DDevice);

    SAFE_RELEASE(g_Direct3D);
}
///////////////////////////////////////////////////////////////
// Entry point of the application
INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, INT)
{
    // Create log window for debug messages
    CreateLogWindow();

    // Print( message into our log console
    Print("Starting application");

    // Create main window
    CreateMainWindow(hInstance);

    // Initialize Direct3D
    CreateDirect3D();

    // Creating our model
    LoadMeshes();

    // Compile shaders
    CompileShaders();

    // Main application loop
    Print("\n");
    Print("Starting event loop");

    g_Timer.Start();

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
        g_Direct3DDevice->Clear(NULL, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, NULL);

        // Begin the scene
        g_Direct3DDevice->BeginScene();

        CalculateLightPosition();

        SetPixelShaderConstants();

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
            Print("Device was been lost - handle");
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
            Print("Resetting Direct3D Device");
            ResetDirect3D();
            g_bNeedReset = false;
        }
    }

    //-------------------DESTROYING CODE-------------------//
    Print("\n");
    Print("Ending event loop");
    Print("\n");

    g_Timer.Stop();

    Print("Releasing resources");
    ClearResources();

    Print("Destroying window");
    DestroyWindow(g_Window);

    Print("\n");
    Print("Application closed successfully, closing window after 3 seconds");
    Sleep(3000);

    return 0;
}
///////////////////////////////////////////////////////////////
