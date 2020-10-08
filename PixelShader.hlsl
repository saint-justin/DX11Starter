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

// Input struct that we get over from the vertex shader
struct VertexToPixel
{
	float4 position	: SV_POSITION;
	float4 color	: COLOR;
	float2 uv		: TEXCOORD;
	float3 normal	: NORMAL;
	float3 worldPos	: POSITION;
};

// Textures and samplers
Texture2D diffuseTexture	: register(t0);
SamplerState basicSampler	: register(s0);
// Texture2D specularTexture	: register(t1); // TODO


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

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	VertexToPixel output;
	input.normal = normalize(input.normal);

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

	return float4(totalLight * input.color.rgb * appliedTexture.rgb, 1);
}