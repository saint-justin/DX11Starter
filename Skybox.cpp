#include "Skybox.h"
// Skybox constructor, to make the baseline version of it
Skybox::Skybox(
	Mesh* _mesh,
	SimplePixelShader* _pixelShader,
	SimpleVertexShader* _vertexShader,
	Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampler,
	Microsoft::WRL::ComPtr<ID3D11Device> _device,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _cubeMap)
{
	mesh = _mesh;
	pixelShader = _pixelShader;
	vertexShader = _vertexShader;
	sampler = _sampler;
	cubeMap = _cubeMap;
	
	// Generate our rasterizer
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	_device->CreateRasterizerState(&rasterizerDesc, rasterizer.GetAddressOf());

	// Generate our depth stentcil state
	D3D11_DEPTH_STENCIL_DESC stencilDesc = {};
	stencilDesc.DepthEnable = true;
	stencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	_device->CreateDepthStencilState(&stencilDesc, stencil.GetAddressOf());
}

void Skybox::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context, Camera* _camera)
{
	// Change our render states
	_context->RSSetState(rasterizer.Get());
	_context->OMSetDepthStencilState(stencil.Get(), 0);

	// Prepare sky-specific shaders for drawing
	pixelShader->SetShader();
	vertexShader->SetShader();

	// Send data to the Vertex Shader stage
	vertexShader->SetMatrix4x4("view", _camera->GetViewMatrix());
	vertexShader->SetMatrix4x4("proj", _camera->GetProjectionMatrix());
	vertexShader->CopyAllBufferData();

	// Ensure the pixel shader has the texture resources it needs
	pixelShader->SetShaderResourceView("cubeMap", cubeMap.Get());
	pixelShader->SetSamplerState("cubeSamplerOptions", sampler.Get());

	// Draw the mesh
	mesh->SetBuffersAndDraw(_context);

	// Reset anything that we perhaps changed
	_context->RSSetState(0);
	_context->OMSetDepthStencilState(0, 0);
}
