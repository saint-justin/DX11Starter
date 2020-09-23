#include "Material.h"

Material::Material(DirectX::XMFLOAT4 _colorTint, Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader, Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader) 
{
	colorTint = _colorTint;
	pixelShader = _pixelShader;
	vertexShader = _vertexShader;
}
