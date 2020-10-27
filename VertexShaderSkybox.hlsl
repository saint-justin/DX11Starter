#include "ShaderShared.hlsli"

// Creating external input cbuffer
cbuffer ExternalData : register(b0)
{
	matrix view;
	matrix proj;
}

// Actual shader calculations
VertexToPixelSkybox main( VertexShaderInput input )
{
	VertexToPixelSkybox output;

	// Remove translation information from the matrix so it stays at 0,0
	matrix viewNoTranslation = view;
	viewNoTranslation._14 = 0;
	viewNoTranslation._24 = 0;
	viewNoTranslation._34 = 0;

	// Calculate and apply world view projection matrix
	matrix wvp = mul(proj, viewNoTranslation);
	output.position = mul(wvp, float4(input.position, 1.0f));
	output.position.z = output.position.w;
	
	// Save off the sample direction as our position
	output.sampleDir = input.position;

	return output;
}