#include "Material.h"

Material::Material(DirectX::XMFLOAT4 _colorTint, 
	SimplePixelShader* _pixelShader, 
	SimpleVertexShader* _vertexShader, 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _textureSRV)
{
	colorTint = _colorTint;
	pixelShader = _pixelShader;
	vertexShader = _vertexShader;
	textureSRV = _textureSRV;
}

DirectX::XMFLOAT4 Material::GetColorTint() { return colorTint; }
SimplePixelShader* Material::GetPixelShader() { return pixelShader; }
SimpleVertexShader* Material::GetVertexShader() { return vertexShader; }
void Material::SetColorTint(DirectX::XMFLOAT4 _colorTint) { colorTint = _colorTint; }	
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetTextureSRV() { return textureSRV; }