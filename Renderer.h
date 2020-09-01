#pragma once

#include "Mesh.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

class Renderer
{
	/*
public:
	Renderer(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context,
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _depthStencilView,
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _backBufferRTV,
		Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader,
		Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader,
		Microsoft::WRL::ComPtr<ID3D11InputLayout> _inputLayout,
		Microsoft::WRL::ComPtr<IDXGISwapChain> _swapChain
	);

	void ClearFrame();
	void Run();
	void SetShadersAndInputLayer();
	void SetBuffers();
	void DrawIndexedMeshes();
	void Present();

	void AddMesh(Mesh* _mesh);

	Mesh meshes[3];
	int meshCount = 0;

private:
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferRTV;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	*/
};

