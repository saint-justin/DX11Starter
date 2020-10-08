#include "Renderer.h"
#include "Mesh.h"
#include "BufferStructs.h"
#include <DirectXMath.h>

// For fancy position displacement
#include <cmath>


Renderer::Renderer() {
	printf("---> Renderer loaded\n");
}

Renderer::~Renderer() {
	printf("---> Renderer unloaded\n");
}

void Renderer::ClearBackground(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferRTV, Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView) {
	// Background color (#000000 black) for clearing
	const float color[4] = { 0.0f, 0.0f, 0.00f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
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
	std::vector<Entity> entities,
	Camera* camera)
{
	for (int i = 0; i < entities.size(); i++)
	{
		SimpleVertexShader* vsData = entities[i].GetMaterial()->GetVertexShader();
		vsData->SetFloat4("colorTint", entities[i].GetMaterial()->GetColorTint());
		vsData->SetMatrix4x4("world", entities[i].GetTransform()->GetWorldMatrix());
		vsData->SetMatrix4x4("view", camera->GetViewMatrix());
		vsData->SetMatrix4x4("proj", camera->GetProjectionMatrix());

		// Set buffers in the input assembler
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, entities[i].GetMesh()->GetVertexBuffer().GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(entities[i].GetMesh()->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

		// Copying to resource
		vsData->CopyAllBufferData();

		// Do the actual drawing
		context->DrawIndexed(entities[i].GetMesh()->GetIndexCount(), 0, 0);
	}
}