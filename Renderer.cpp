#include "Renderer.h"

Renderer::Renderer() {
	printf("---> Renderer loaded");
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

void Renderer::DrawMeshes(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader, Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		// Set the vertex and pixel shaders to use for the next Draw() command
		//  - These don't technically need to be set every frame
		//  - Once you start applying different shaders to different objects,
		//    you'll need to swap the current shaders before each draw
		context->VSSetShader(vertexShader.Get(), 0, 0);
		context->PSSetShader(pixelShader.Get(), 0, 0);

		// Set buffers in the input assembler
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, meshes[i]->GetVertexBuffer().GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(meshes[i]->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

		// Do the actual drawing
		context->DrawIndexed(meshes[i]->GetIndexCount(), 0, 0);
		//printf("Index Count %d \n", meshes[i]->GetIndexCount());
	}
}