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
	renderer = new Renderer();
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
	delete pixelShader;
	delete vertexShader;

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
	LoadShaders();
	CreateBasicGeometry();
	
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Setting up light info
	light1.ambientColor =	{ 0.01f, 0.01f, 0.01f };
	light1.diffuseColor =	{ 0.1f, 0.1f, 0.1f };
	light1.direction =		{ 1.0f, -1.0f, 0.0f };

	light2.ambientColor = { 0.01f, 0.01f, 0.01f };
	light2.diffuseColor = { 0.4f, 0.1f, 0.1f };
	light2.direction = { 1.0f, -0.20f, -0.3f };

	light3.ambientColor = { 0.00f, 0.02f, 0.1f };
	light3.diffuseColor = { 0.7f, 1.0f, 0.7f };
	light3.direction = { 0.0f, 0.5f, -1.0f };
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// --------------------------------------------------------
void Game::LoadShaders()
{
	pixelShader = new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShader.cso").c_str());
	vertexShader = new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"VertexShader.cso").c_str());
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

	// Store the materials and meshes
	materials.push_back(redMat);
	materials.push_back(greenMat);
	materials.push_back(blueMat);
	materials.push_back(whiteMat);
	meshes.push_back(new Mesh(GetFullPathTo("/Assets/sphere.obj").c_str(), device));
	meshes.push_back(new Mesh(GetFullPathTo("/Assets/cube.obj").c_str(), device));
	meshes.push_back(new Mesh(GetFullPathTo("/Assets/helix.obj").c_str(), device));
	meshes.push_back(new Mesh(GetFullPathTo("/Assets/cylinder.obj").c_str(), device));
	meshes.push_back(new Mesh(GetFullPathTo("/Assets/cone.obj").c_str(), device));
	meshes.push_back(new Mesh(GetFullPathTo("/Assets/torus.obj").c_str(), device));

	// Setting up meshes and materials in entities then spreading 'em out
	Entity ent1 = Entity(meshes[0], materials[0]);
	ent1.GetTransform()->SetPosition(-3.0f, 0.0f, 0.0f);

	Entity ent2 = Entity(meshes[1], materials[1]);
	ent2.GetTransform()->SetPosition(-1.5f, 0.0f, 0.0f);
	
	Entity ent3 = Entity(meshes[2], materials[2]);
	ent3.GetTransform()->SetPosition(0.0f, 0.0f, 0.0f);
	ent3.GetTransform()->SetScale(0.75f, 0.75f, 0.75f);
	
	Entity ent4 = Entity(meshes[3], materials[3]);
	ent4.GetTransform()->SetPosition(1.5f, 0.0f, 0.0f);
	
	Entity ent5 = Entity(meshes[4], materials[1]);
	ent5.GetTransform()->SetPosition(3.0f, 0.0f, 0.0f);
	
	Entity ent6 = Entity(meshes[5], materials[1]);
	ent6.GetTransform()->SetPosition(4.5f, 0.0f, 0.0f);

	// Setting the entities into the entity array
	entities.push_back(ent1);
	entities.push_back(ent2);
	entities.push_back(ent3);
	entities.push_back(ent4);
	entities.push_back(ent5);
	entities.push_back(ent6);

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

	// Passing lighting information
	pixelShader->SetData("directionalLight1", &light1, sizeof(DirectionalLight));
	pixelShader->SetData("directionalLight2", &light2, sizeof(DirectionalLight));
	pixelShader->SetData("directionalLight3", &light3, sizeof(DirectionalLight));
	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();

	renderer->DrawMeshes(context, entities, camera);

	// Present the back buffer to the user
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain, the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}

