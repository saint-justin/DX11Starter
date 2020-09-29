#pragma once

#include "Mesh.h"
#include "BufferStructs.h"
#include "Entity.h"
#include "Camera.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <DirectXMath.h>

class Renderer
{
public:
	Renderer(Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader, Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader);
	~Renderer();

	void ClearBackground(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferRTV,
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView);

	void DrawMeshes(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer,
		std::vector<Entity> entities,
		Camera* camera);
		
	std::vector<Mesh*> meshes;

private:
	float counter;
};

