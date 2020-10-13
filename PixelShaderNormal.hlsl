
#include "ShaderShared.hlsli"

// Cbuffer struct for passing in exterior data
cbuffer externalData : register(b0)
{
	float3 environmentAmbient;
	float specularIntensity;

	float3 lightColor;
	float3 lightDir;
	float3 lightIntensitiy;

	float3 pointLightPos;
	float3 pointLightColor;
	float3 pointLightIntensity;

	float3 cameraPos;
};

// Textures and samplers
Texture2D diffuseTexture	: register(t0);
Texture2D normalTexture		: register(t1);
SamplerState basicSampler	: register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// --------------------------------------------------------
float4 main(VertexToPixelWithTangent input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	float3 unpackedNormal = normalTexture.Sample(basicSampler, input.uv).rgb * 2 - 1;
	input.normal = mul(unpackedNormal, createTBNMatrix(input.normal, input.tangent));

	float3 toCamera = normalize(cameraPos - input.worldPos);
	float3 normalizedLightDir = normalize(lightDir);

	// Directional light
	float directionalDiffuse = Diffuse(input.normal, normalizedLightDir);
	float directionalSpecular = Specular(input.normal, normalizedLightDir, toCamera, 64.0f);
	float3 directionalLightCombined = (directionalDiffuse + directionalSpecular) * lightDir * lightColor;

	// Point light
	float3 pointLightDir = normalize(input.worldPos - pointLightPos);
	float pointLightDiffuse = Diffuse(input.normal, pointLightPos);
	float pointLightSpecular = Specular(input.normal, pointLightPos, toCamera, 64.0f);
	float3 pointLightCombined = (pointLightDiffuse + pointLightSpecular) * pointLightIntensity * pointLightColor;

	// Combining point and directional lights
	float3 totalLight = environmentAmbient + pointLightCombined + directionalLightCombined;
	float4 appliedTexture = diffuseTexture.Sample(basicSampler, input.uv);


	return appliedTexture;

	return float4(totalLight * input.color.rgb * appliedTexture.rgb, 1);
}