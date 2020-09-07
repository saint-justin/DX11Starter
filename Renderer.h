#pragma once

#include "Mesh.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

class Renderer
{
public:
	Renderer();

	void ClearBackground(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferRTV,
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView);

	void DrawMeshes(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, 
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader, 
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader);

	
	std::vector<Mesh*> meshes;

private:

};

