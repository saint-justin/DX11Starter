#include "Renderer.h"
/*
// Initializing the renderer
Renderer::Renderer(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context, 
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _depthStencilView, 
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _backBufferRTV, 
	Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader, 
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader, 
	Microsoft::WRL::ComPtr<ID3D11InputLayout> _inputLayout, 
	Microsoft::WRL::ComPtr<IDXGISwapChain> _swapChain)
{	
	context = _context;
	depthStencilView = _depthStencilView;
	backBufferRTV = _backBufferRTV;
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
	inputLayout = _inputLayout;
	swapChain = _swapChain;

}

void Renderer::ClearFrame()
{
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

// Pulls together all the other functions
void Renderer::Run() {
	// Clear the frame
	ClearFrame();

	// Loop through all meshes and complete their logic individually
	SetShadersAndInputLayer();
	SetBuffers();
	DrawIndexedMeshes();

	// Actually render
	Present();
}

void Renderer::SetShadersAndInputLayer()
{
	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	context->VSSetShader(vertexShader.Get(), 0, 0);
	context->PSSetShader(pixelShader.Get(), 0, 0);

	// Ensure the pipeline knows how to interpret the data (numbers)
	// from the vertex buffer.  
	// - If all of your 3D models use the exact same vertex layout,
	//    this could simply be done once in Init()
	// - However, this isn't always the case (but might be for this course)
	context->IASetInputLayout(inputLayout.Get());
}

void Renderer::SetBuffers()
{
	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	//  - for this demo, this step *could* simply be done once during Init(),
	//    but I'm doing it here because it's often done multiple times per frame
	//    in a larger application/game
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, meshes[0].GetVertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(meshes[0].GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
}

void Renderer::DrawIndexedMeshes()
{
	context->DrawIndexed(meshes[0].GetIndexCount(), 0, 0);
	printf("Index Count %d \n", meshes[0].GetIndexCount());
}

void Renderer::Present()
{
	// Present the back buffer to the user
//  - Puts the final frame we're drawing into the window so the user can see it
//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}

void Renderer::AddMesh(Mesh* _mesh)
{
	meshes[meshCount] = *_mesh;
	meshCount++;
}*/
