#include "Game.h"
#include "Vertex.h"
#include "Renderer.h"

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif

	// Setting up renderer and camera
	renderer = new Renderer(vertexShader, pixelShader);
	camera = new Camera(0, 0, -5, (float)this->width / this->height);
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Clean up all of our normal pointers to items on the heap

	delete renderer;
	delete camera;

	for (int i = 0; i < meshes.size(); i++) {
		delete meshes[i];
	}

	for (int i = 0; i < materials.size(); i++) {
		delete materials[i];
	}
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateBasicGeometry();
	
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Ensure the pipeline knows how to interpret the data (numbers) from the vertex buffer.  
	context->IASetInputLayout(inputLayout.Get());

	// Creating external vert shader buffer
	unsigned int size = sizeof(VertexShaderExternalData);
	size = (size + 15) / 16 * 16;
	CD3D11_BUFFER_DESC cbDesc = {};
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.ByteWidth = size;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	device->CreateBuffer(&cbDesc, 0, vsConstantBuffer.GetAddressOf());
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	// Blob for reading raw data
	// - This is a simplified way of handling raw data
	ID3DBlob* shaderBlob;

	// Read our compiled vertex shader code into a blob
	// - Essentially just "open the file and plop its contents here"
	D3DReadFileToBlob(
		GetFullPathTo_Wide(L"VertexShader.cso").c_str(), // Using a custom helper for file paths
		&shaderBlob);

	// Create a vertex shader from the information we
	// have read into the blob above
	// - A blob can give a pointer to its contents, and knows its own size
	device->CreateVertexShader(
		shaderBlob->GetBufferPointer(), // Get a pointer to the blob's contents
		shaderBlob->GetBufferSize(),	// How big is that data?
		0,								// No classes in this shader
		vertexShader.GetAddressOf());	// The address of the ID3D11VertexShader*


	// Create an input layout that describes the vertex format
	// used by the vertex shader we're using
	//  - This is used by the pipeline to know how to interpret the raw data
	//     sitting inside a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the blob above)
	D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

	// Set up the first element - a position, which is 3 float values
	inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
	inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
	inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

	// Set up the second element - a color, which is 4 more float values
	inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
	inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
	inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

	// Create the input layout, verifying our description against actual shader code
	device->CreateInputLayout(
		inputElements,					// An array of descriptions
		2,								// How many elements in that array
		shaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
		shaderBlob->GetBufferSize(),	// Size of the shader code that uses this layout
		inputLayout.GetAddressOf());	// Address of the resulting ID3D11InputLayout*



	// Read and create the pixel shader
	//  - Reusing the same blob here, since we're done with the vert shader code
	D3DReadFileToBlob(
		GetFullPathTo_Wide(L"PixelShader.cso").c_str(), // Using a custom helper for file paths
		&shaderBlob);

	device->CreatePixelShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		0,
		pixelShader.GetAddressOf());
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	// Create some temporary variables to represent colors
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 white = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Generating materials to be used by different meshes
	Material* redMat = new Material(red, pixelShader, vertexShader);
	Material* greenMat = new Material(green, pixelShader, vertexShader);
	Material* blueMat = new Material(blue, pixelShader, vertexShader);
	Material* whiteMat = new Material(white, pixelShader, vertexShader);

	materials.push_back(redMat);
	materials.push_back(greenMat);
	materials.push_back(blueMat);
	materials.push_back(whiteMat);

	// Generating three basic meshes that will be used to test entities
	Vertex vertices01[] =
	{
		{ XMFLOAT3(+0.0f, +0.5f, +0.0f), red },		//triangle top
		{ XMFLOAT3(+0.5f, +0.25f, +0.0f), blue },	//triangle left
		{ XMFLOAT3(+0.5f, +0.5f, +0.0f), white },	//triangle  right
	};

	Vertex vertices02[] =
	{
		{ XMFLOAT3(-0.4f, -0.4f, +0.0f), red },		//top right
		{ XMFLOAT3(-0.4f, -0.6f, +0.0f), green },	//bottom right
		{ XMFLOAT3(-0.6f, -.04f, +0.0f), blue },	//top left
		{ XMFLOAT3(-0.6f, -0.6f, +0.0f), white },	//bottom left
	};

	Vertex vertices03[] =
	{
		{ XMFLOAT3(+0.9f, +0.9f, +0.0f), white },		//triangle top right
		{ XMFLOAT3(+0.9f, +0.5f, +0.0f), white },	//triangle bottom right
		{ XMFLOAT3(+0.5f, +0.9f, +0.0f), white },	//triangle top left
	};


	unsigned int indices01[] = { 1, 0, 2 };
	unsigned int indices02[] = { 3, 2, 1, 2, 0, 1 };
	unsigned int indices03[] = { 0, 1, 2 };

	Mesh* mesh1 = new Mesh(vertices01, sizeof(vertices01), indices01, sizeof(indices01), device);
	Mesh* mesh2 = new Mesh(vertices02, sizeof(vertices02), indices02, sizeof(indices02), device);
	Mesh* mesh3 = new Mesh(vertices03, sizeof(vertices03), indices03, sizeof(indices03), device);

	meshes.push_back(mesh1);
	meshes.push_back(mesh2);
	meshes.push_back(mesh3);

	// Standard, normal meshes
	Entity ent1 = Entity(meshes[0], materials[0]);
	Entity ent2 = Entity(meshes[1], materials[1]);
	Entity ent3 = Entity(meshes[2], materials[2]);

	// Fancy meshes
	Entity ent1a = Entity(meshes[0], materials[0]);
	ent1a.GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);

	Entity ent1b = Entity(meshes[0], materials[0]);
	ent1b.GetTransform()->SetScale(1.2f, 1.2f, 1.2f);

	// Setting the entities into the entity array
	entities.push_back(ent1);
	entities.push_back(ent2);
	entities.push_back(ent3);
	entities.push_back(ent1a);
	entities.push_back(ent1b);

}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Adjust the camera matrix
	camera->UpdateProjectionMatrix((float)this->width / this->height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	// Update the camera
	camera->Update(deltaTime, this->hWnd);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Clear the background then draw the meshes
	renderer->ClearBackground(context, backBufferRTV, depthStencilView);
	renderer->DrawMeshes(context, vsConstantBuffer, entities, camera);

	// Present the back buffer to the user
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain, the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}

