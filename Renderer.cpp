#include "Renderer.h"
#include "Mesh.h"
#include "BufferStructs.h"
#include <DirectXMath.h>

// For fancy position displacement
#include <cmath>


Renderer::Renderer(Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader, Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader) {
	printf("---> Renderer loaded\n");

	counter = 0.0f;
}

Renderer::~Renderer() {
	for (int i = 0; i < meshes.size(); i++) {
		delete meshes[i];
	}
	printf("---> Renderer unloaded\n");
}

void Renderer::ClearBackground(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferRTV, Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView) {
	// Background color (#000 black) for clearing
	const float color[4] = { 0.0f, 0.0f, 0.00f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
}

// Cycles through all the meshes the renderer has a references to and renders them on the rendertarget to get presented to the screen
void Renderer::DrawMeshes(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
	Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer,
	std::vector<Entity> entities,
	Camera* camera)
{
	// Basically substituting a real timer for this garbage for now
	counter += 0.01f;

	// Making frame-by-frame adjustments to the entities
	entities[1].GetTransform()->MoveAbsolute(std::sin(counter) * 0.005f, 0.0f, 0.0f);
	entities[3].GetTransform()->SetRotation(0.0f, 0.0f, counter * 1.5f);
	entities[4].GetTransform()->SetScale(std::sin(counter) * 1.5f, std::sin(counter) * 1.5f, 1.0f);

	// Setting up the vertex shader's external data
	VertexShaderExternalData vsData;
	vsData.colorTint = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vsData.projMatrix = camera->GetProjectionMatrix();
	vsData.viewMatrix = camera->GetViewMatrix();

	for (int i = 0; i < entities.size(); i++)
	{
		// Adjust the world matrix for this specific entity
		vsData.worldMatrix = entities[i].GetTransform()->GetWorldMatrix();

		// Set the vertex and pixel shaders to use
		context->VSSetShader(entities[i].GetMaterial()->vertexShader.Get(), 0, 0);
		context->PSSetShader(entities[i].GetMaterial()->pixelShader.Get(), 0, 0);

		// Set buffers in the input assembler
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, entities[i].GetMesh()->GetVertexBuffer().GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(entities[i].GetMesh()->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

		// Copying to resource
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		context->Map(vsConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
		context->Unmap(vsConstantBuffer.Get(), 0);
		context->VSSetConstantBuffers(0, 1, vsConstantBuffer.GetAddressOf());

		// Do the actual drawing
		context->DrawIndexed(entities[i].GetMesh()->GetIndexCount(), 0, 0);
	}
}