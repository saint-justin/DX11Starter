
// Struct representing the data we expect to receive from earlier pipeline stages
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float4 color		: COLOR;
	float3 normal		: NORMAL;
};

// Struct representing the data structure we're using for directional lights
struct DirectionalLight
{
	float3 ambientColor;
	float3 diffuseColor;
	float3 direction;
};

// Constant buffer where we pass in light information
cbuffer LightData: register(b0)
{
	DirectionalLight directionalLight1;
	DirectionalLight directionalLight2;
	DirectionalLight directionalLight3;
}

// Calculates and saturates n dot l
float ndotl(float n, float l)
{
	return saturate(dot(n, normalize(-l)));
}


// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	VertexToPixel output;
	input.normal = normalize(input.normal);

	float ndl1 = ndotl(input.normal, directionalLight1.direction);
	float ndl2 = ndotl(input.normal, directionalLight2.direction);
	float ndl3 = ndotl(input.normal, directionalLight3.direction);

	float3 diffuse1 = directionalLight1.diffuseColor * input.color.rgb * ndl1;
	float3 diffuse2 = directionalLight2.diffuseColor * input.color.rgb * ndl2;
	float3 diffuse3 = directionalLight3.diffuseColor * input.color.rgb * ndl3;

	float3 ambient1 = directionalLight1.ambientColor.rgb * input.color.rgb;
	float3 ambient2 = directionalLight2.ambientColor.rgb * input.color.rgb;
	float3 ambient3 = directionalLight3.ambientColor.rgb * input.color.rgb;

	return float4(diffuse1 + diffuse2 + diffuse3 + ambient1 + ambient2 + ambient3, 1);
}