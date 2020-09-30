#pragma once
#include "Material.h"
#include "SimpleShader.h"

#include <wrl/client.h> 
#include <d3d11.h>
#include <DirectXMath.h>

class Material
{
public:
	Material(
		DirectX::XMFLOAT4 _colorTint, 
		SimplePixelShader* _pixelShader, 
		SimpleVertexShader* vertexShader
	);

	DirectX::XMFLOAT4 GetColorTint();
	SimplePixelShader* GetPixelShader();
	SimpleVertexShader* GetVertexShader();

private:
	DirectX::XMFLOAT4 colorTint;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShader;
};

