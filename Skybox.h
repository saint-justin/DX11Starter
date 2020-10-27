#pragma once
#include <wrl/client.h> 
#include <d3d11.h>
#include "Mesh.h"
#include "SimpleShader.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
#include "Camera.h"

class Skybox
{
public:
	// Skybox constructor, to make the baseline version of it
	Skybox(
		Mesh* _mesh,
		SimplePixelShader* _pixelShader,
		SimpleVertexShader* _vertexShader,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler,
		Microsoft::WRL::ComPtr<ID3D11Device> _device,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _cubeMap
	);

	// Actually draws our sky
	void Draw(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		Camera* camera
	);

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMap;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> stencil;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer;
	Mesh* mesh;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShader;
};

