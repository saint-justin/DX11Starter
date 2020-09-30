#include "Material.h"

Material::Material(DirectX::XMFLOAT4 _colorTint, SimplePixelShader* _pixelShader, SimpleVertexShader* _vertexShader)
{
	colorTint = _colorTint;
	pixelShader = _pixelShader;
	vertexShader = _vertexShader;
}

DirectX::XMFLOAT4 Material::GetColorTint() { return colorTint; }
SimplePixelShader* Material::GetPixelShader() { return pixelShader; }
SimpleVertexShader* Material::GetVertexShader() { return vertexShader; }