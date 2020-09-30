#pragma once

#include <DirectXMath.h>

// --------------------------------------------------------
// A custom vertex definition
//
// You will eventually ADD TO this, and/or make more of these!
// --------------------------------------------------------
struct Vertex
{
	DirectX::XMFLOAT3 position;	    // The position of the vertex
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};