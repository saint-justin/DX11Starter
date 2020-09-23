#pragma once
#include <wrl/client.h> 
#include <d3d11.h>
#include <DirectXMath.h>

class Material
{
public:
	Material(DirectX::XMFLOAT4 _colorTint, Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader, Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader);
private:
	DirectX::XMFLOAT4 colorTint;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
};

