#ifndef __GGP_SHADER_INCLUDES__
#define __GGP_SHADER_INCLUDES__

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

#endif