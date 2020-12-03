#include "Material.h"

Material::Material(DirectX::XMFLOAT4 _colorTint, 
	SimplePixelShader* _pixelShader, 
	SimpleVertexShader* _vertexShader, 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _textureSRV,
	int _renderPriority)
{
	colorTint = _colorTint;
	pixelShader = _pixelShader;
	vertexShader = _vertexShader;
	albedoMapSRV = _textureSRV;
	normalMapSRV = nullptr;
	hasNormalMap = false;
	renderPriority = _renderPriority;
}

Material::Material(DirectX::XMFLOAT4 _colorTint,
	SimplePixelShader* _pixelShader,
	SimpleVertexShader* _vertexShader,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _textureSRV,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _normalMap,
	int _renderPriority)
{
	colorTint = _colorTint;
	pixelShader = _pixelShader;
	vertexShader = _vertexShader;
	albedoMapSRV = _textureSRV;
	normalMapSRV = _normalMap;
	hasNormalMap = true;
	renderPriority = _renderPriority;
}

DirectX::XMFLOAT4 Material::GetColorTint() { return colorTint; }
SimplePixelShader* Material::GetPixelShader() { return pixelShader; }
SimpleVertexShader* Material::GetVertexShader() { return vertexShader; }
void Material::SetColorTint(DirectX::XMFLOAT4 _colorTint) { colorTint = _colorTint; }	
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetTextureSRV() { return albedoMapSRV; }
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetNormalMap() { return normalMapSRV; }