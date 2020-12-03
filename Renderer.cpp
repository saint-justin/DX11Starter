#include "Renderer.h"
#include "Mesh.h"
#include "BufferStructs.h"
#include <DirectXMath.h>
#include <typeinfo>

// For fancy position displacement
#include <cmath>


Renderer::Renderer() {
	printf("---> Renderer loaded\n");
}

Renderer::~Renderer() {
	printf("---> Renderer unloaded\n");
}

// Clears the background every frame to pure black
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
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler,
	std::vector<Entity> entities,
	Camera* camera)
{
	for (int i = 0; i < entities.size(); i++)
	{
		// Set sampler, diffuse, and maybe normal textures
		entities[i].GetMaterial()->GetPixelShader()->SetSamplerState("basicSampler", sampler.Get());
		entities[i].GetMaterial()->GetPixelShader()->SetShaderResourceView("diffuseTexture", entities[i].GetMaterial()->GetTextureSRV().Get());
		if (entities[i].GetMaterial()->hasNormalMap) {
			entities[i].GetMaterial()->GetPixelShader()->SetShaderResourceView("normalTexture", entities[i].GetMaterial()->GetNormalMap().Get());
		}

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

		// Set the shaders and sampler state
		entities[i].GetMaterial()->GetVertexShader()->SetShader();	// TODO: Clump together items to render based on which shader type they are
		entities[i].GetMaterial()->GetPixelShader()->SetShader();
		entities[i].GetMaterial()->GetPixelShader()->CopyAllBufferData();

		// Do the actual drawing
		context->DrawIndexed(entities[i].GetMesh()->GetIndexCount(), 0, 0);
	}
}

// Cycles through all the meshes the renderer has a references to and renders them on the rendertarget to get presented to the screen
void Renderer::DrawMeshesQueued(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler,
	Camera* camera)
{
	int currentPriority = -1;
	for (int i = 0; i < renderQueue.size(); i++)
	{
		// Set sampler, diffuse, and maybe normal textures
		renderQueue[i].GetMaterial()->GetPixelShader()->SetSamplerState("basicSampler", sampler.Get());
		renderQueue[i].GetMaterial()->GetPixelShader()->SetShaderResourceView("diffuseTexture", renderQueue[i].GetMaterial()->GetTextureSRV().Get());
		if (renderQueue[i].GetMaterial()->hasNormalMap) {
			renderQueue[i].GetMaterial()->GetPixelShader()->SetShaderResourceView("normalTexture", renderQueue[i].GetMaterial()->GetNormalMap().Get());
		}

		SimpleVertexShader* vsData = renderQueue[i].GetMaterial()->GetVertexShader();
		vsData->SetFloat4("colorTint", renderQueue[i].GetMaterial()->GetColorTint());
		vsData->SetMatrix4x4("world", renderQueue[i].GetTransform()->GetWorldMatrix());
		vsData->SetMatrix4x4("view", camera->GetViewMatrix());
		vsData->SetMatrix4x4("proj", camera->GetProjectionMatrix());

		// Set buffers in the input assembler
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, renderQueue[i].GetMesh()->GetVertexBuffer().GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(renderQueue[i].GetMesh()->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

		// Copying to resource
		vsData->CopyAllBufferData();

		// Set the shaders and sampler state, but only if we need to
		if (currentPriority != renderQueue[i].renderPriority) {
			currentPriority = renderQueue[i].renderPriority;
			renderQueue[i].GetMaterial()->GetPixelShader()->SetShader();
			renderQueue[i].GetMaterial()->GetPixelShader()->CopyAllBufferData();
		}
		renderQueue[i].GetMaterial()->GetVertexShader()->SetShader();	// TODO: Clump together items to render based on which shader type they are

		// Do the actual drawing
		context->DrawIndexed(renderQueue[i].GetMesh()->GetIndexCount(), 0, 0);
	}
}

void Renderer::GenerateRenderQueue(std::vector<Entity> entities)
{
	// Clear out the old queue and set it up to be filled with a new one
	renderQueue.clear();

	// Fill the queue with all the existing entities
	for (int i = 0; i < entities.size(); i++)
		renderQueue.push_back(entities[i]);

	// Sort that queue based on its priority
	std::sort(renderQueue.begin(), renderQueue.end());
	dirty = false;
}

void Renderer::SetDirty() { dirty = true; }
bool Renderer::GetDirty() { return dirty; }
