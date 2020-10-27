#include "ShaderShared.hlsli"

// Creating cbuffer
cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	matrix world;
	matrix view;
	matrix proj;
}

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixelWithTangent main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixelWithTangent output;

	// Calculate and apply world view projection matrix
	matrix wvp = mul(proj, mul(view, world));
	output.position = mul(wvp, float4(input.position, 1.0f));
	output.normal = normalize(mul((float3x3)world, input.normal));			// TODO: Update the world position to be the inverse transpose when passing in

	// Calculate vert's world position
	output.worldPos = mul(world, float4(input.position, 1.0f)).xyz;

	// Pass the color, tangent, and uv through 
	output.color = colorTint;
	output.tangent = input.tangent;
	output.uv = input.uv;


	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}