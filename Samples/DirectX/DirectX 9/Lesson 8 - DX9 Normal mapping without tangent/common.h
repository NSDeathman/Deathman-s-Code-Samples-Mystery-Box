// Matrix uniforms for transformation and projection
uniform float4x4 matWorld;                  // World matrix: transforms object space to world space
uniform float4x4 matView;                   // View matrix: transforms world space to camera/view space
uniform float4x4 matProjection;             // Projection matrix: transforms view space to clip space
uniform float4x4 matWorldView;              // Combined world and view matrix
uniform float4x4 matWorldViewProjection;    // Combined world, view, and projection matrix

// Texture sampler for the albedo (color) texture
uniform sampler2D smpAlbedo : register(s0); // Sampler for accessing the albedo texture, bound to register s0

uniform float4 LightPosition;