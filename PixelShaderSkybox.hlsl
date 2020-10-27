#include "ShaderShared.hlsli"

// Setting textures and sampler registers
TextureCube	cubeMap : register(t0);
SamplerState cubeSamplerOptions	: register(s0);

float4 main(VertexToPixelSkybox input) : SV_TARGET
{
	return cubeMap.Sample(cubeSamplerOptions, input.sampleDir);
}