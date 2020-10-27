#ifndef __GGP_SHARED_SHADER_INFO__
#define __GGP_SHARED_SHADER_INFO__

// ---------------------- STRUCTS ----------------------

// Struct representing a single vertex worth of data, the input for our vertex shader
struct VertexShaderInput
{
	float3 position		: POSITION;     // XYZ position
	float3 normal		: NORMAL;       // Vertex Normal
	float3 tangent		: TANGENT;		// Vector tangent to the surface
	float2 uv			: TEXCOORD;     // Vertex UV info
};

// Struct to pass information from the vertex shader to the pixel shader
struct VertexToPixel
{
	float4 position	: SV_POSITION;		// Position of pixel 
	float4 color	: COLOR;			// Base color of pixel
	float2 uv		: TEXCOORD;			// UV coord of the pixel
	float3 normal	: NORMAL;			// Normal at the pixel's position
	float3 worldPos	: POSITION;			// Position of the pixel in world space
};

// Struct to pass information from the vertex shader to the pixel shader
struct VertexToPixelWithTangent
{
	float4 position	: SV_POSITION;		// Position of pixel 
	float4 color	: COLOR;			// Base color of pixel
	float2 uv		: TEXCOORD;			// UV coord of the pixel
	float3 normal	: NORMAL;			// Normal at the pixel's position
	float3 worldPos	: POSITION;			// Position of the pixel in world space
	float3 tangent	: TANGENT;
};

// Struct to pass information from the vertex shader to the pixel shader for the skybox
struct VertexToPixelSkybox
{
	float4 position  : SV_POSITION;
	float3 sampleDir : DIRECTION;
};

// ---------------------- HELPER FUNCTIONS ----------------------

// Calculates the diffuse amount given the surface and direction from light
float Diffuse(float3 normal, float3 lightDir)
{
	return saturate(dot(normal, -lightDir));
}

// Calculates the specular we'd get from a Phong model
float Specular(float3 normal, float3 lightDir, float3 toCamera, float specular)
{
	return pow(saturate(dot(reflect(lightDir, normal), toCamera)), specular);
}


float3x3 createTBNMatrix(float3 normal, float3 tangent) 
{
	normal = normalize(normal);
	tangent = normalize(tangent);
	tangent = normalize(tangent - normal * dot(tangent, normal));
	float3 bitangent = cross(tangent, normal);
	return float3x3(tangent, bitangent, normal);
}
#endif