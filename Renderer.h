#pragma once

#include "Mesh.h"
#include "BufferStructs.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

class Renderer
{
public:
	Renderer();
	~Renderer();

	void ClearBackground(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferRTV,
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView);

	void DrawMeshes(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader,
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader,
		Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer,
		VertexShaderExternalData vsData);
		
	std::vector<Mesh*> meshes;

private:

};

