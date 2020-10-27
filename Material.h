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
		SimpleVertexShader* vertexShader,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _textureSRV
	);
	Material(
		DirectX::XMFLOAT4 _colorTint,
		SimplePixelShader* _pixelShader,
		SimpleVertexShader* vertexShader,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _textureSRV,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _normalMap
	);

	DirectX::XMFLOAT4 GetColorTint();
	SimplePixelShader* GetPixelShader();
	SimpleVertexShader* GetVertexShader();
	void SetColorTint(DirectX::XMFLOAT4 tint);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTextureSRV();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetNormalMap();
	bool hasNormalMap;

private:
	DirectX::XMFLOAT4 colorTint;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShader;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> albedoMapSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMapSRV;

};

