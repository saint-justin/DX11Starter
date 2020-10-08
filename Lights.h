#pragma once
#include <DirectXMath.h>

struct DirectionalLight
{
	DirectX::XMFLOAT3 ambientColor;
	float padding1;
	DirectX::XMFLOAT3 diffuseColor;
	float padding2;
	DirectX::XMFLOAT3 direction;
};

struct PointLight
{
	DirectX::XMFLOAT3 ambientColor;
	float padding1;
	DirectX::XMFLOAT3 diffusedColor;
	float padding;
	DirectX::XMFLOAT3 position;
	// TODO: Add in attenuation radius in the future
};