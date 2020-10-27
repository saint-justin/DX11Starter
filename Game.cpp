#include "Game.h"
#include "Vertex.h"
#include "Renderer.h"

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include "WICTextureLoader.h"

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

	// Yeet all those shaders
	delete pixelShader;
	delete vertexShader;
	delete pixelShaderWithNormals;
	delete vertexShaderWithNormals;
	delete pixelShaderSkybox;
	delete vertexShaderSkybox;
	delete skybox;


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
	// Load the actual shaders
	LoadShaders();
	
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Load in our textures
	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/cushion.png").c_str(),
		nullptr,
		cushionDiffuseMap.GetAddressOf()
	);
	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/cushion_normals.png").c_str(),
		nullptr,
		cushionNormalMap.GetAddressOf()
	);
	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/rock.png").c_str(),
		nullptr,
		rockDiffuseMap.GetAddressOf()
	);
	CreateWICTextureFromFile(
		device.Get(),
		context.Get(),
		GetFullPathTo_Wide(L"../../Assets/rock_normals.png").c_str(),
		nullptr,
		rockNormalMap.GetAddressOf()
	);

	// Set up our sampler description
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 8;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	
	// Create the sampler
	device->CreateSamplerState(&sampDesc, samplerState.GetAddressOf());
	CreateBasicGeometry();

	// Load the cubemap then create our skybox with it
	cubeMap = CreateCubemap(
		GetFullPathTo_Wide(L"../../Assets/Skybox/posx.jpg").c_str(),
		GetFullPathTo_Wide(L"../../Assets/Skybox/negx.jpg").c_str(),
		GetFullPathTo_Wide(L"../../Assets/Skybox/posy.jpg").c_str(),
		GetFullPathTo_Wide(L"../../Assets/Skybox/negy.jpg").c_str(),
		GetFullPathTo_Wide(L"../../Assets/Skybox/posz.jpg").c_str(),
		GetFullPathTo_Wide(L"../../Assets/Skybox/negz.jpg").c_str()
	);
	skybox = new Skybox(meshes[1], pixelShaderSkybox, vertexShaderSkybox, samplerState, device, cubeMap);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// --------------------------------------------------------
void Game::LoadShaders()
{
	pixelShader = new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShader.cso").c_str());
	pixelShaderWithNormals = new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShaderNormal.cso").c_str());
	pixelShaderSkybox = new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShaderSkybox.cso").c_str());
	vertexShader = new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"VertexShader.cso").c_str());
	vertexShaderSkybox = new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"VertexShaderSkybox.cso").c_str());
	vertexShaderWithNormals = new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"VertexShaderNormal.cso").c_str());
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
	Material* cushionNoNormal = new Material(white, pixelShader, vertexShader, cushionDiffuseMap);
	Material* cushionNormal = new Material(white, pixelShaderWithNormals, vertexShaderWithNormals, cushionDiffuseMap, cushionNormalMap);
	Material* rocksNoNormal = new Material(white, pixelShader, vertexShader, rockDiffuseMap);
	Material* rocksNormal = new Material(white, pixelShaderWithNormals, vertexShaderWithNormals, rockDiffuseMap, rockNormalMap);

	// Store the materials and meshes
	materials.push_back(cushionNoNormal);
	materials.push_back(cushionNormal);
	materials.push_back(rocksNoNormal);
	materials.push_back(rocksNormal);
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/sphere.obj").c_str(), device));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/cube.obj").c_str(), device));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/helix.obj").c_str(), device));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/cylinder.obj").c_str(), device));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/cone.obj").c_str(), device));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/torus.obj").c_str(), device));

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

	// Draw the skybox
	skybox->Draw(context, camera);

	// Push in pixel shader information
	pixelShader->SetFloat3("lightColor", DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f));
	pixelShader->SetFloat3("lightDir", DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f));
	pixelShader->SetFloat("lightIntensity", 4.0f);

	pixelShader->SetFloat3("pointLightColor", DirectX::XMFLOAT3(0, 0, 2.0f));
	pixelShader->SetFloat3("pointLightPos", DirectX::XMFLOAT3(-10.0f, -10.0f, -10.0f));
	pixelShader->SetFloat("pointLightIntensity", 4.0f);

	pixelShader->SetFloat3("environmentAmbient", DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f));
	pixelShader->SetFloat("specularIntensity", 2.0f);

	// Push in lighting information for pixel shader with normal maps
	pixelShaderWithNormals->SetFloat3("lightColor", DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f));
	pixelShaderWithNormals->SetFloat3("lightDir", DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f));
	pixelShaderWithNormals->SetFloat("lightIntensity", 4.0f);

	pixelShaderWithNormals->SetFloat3("pointLightColor", DirectX::XMFLOAT3(0, 0, 2.0f));
	pixelShaderWithNormals->SetFloat3("pointLightPos", DirectX::XMFLOAT3(-10.0f, -10.0f, -10.0f));
	pixelShaderWithNormals->SetFloat("pointLightIntensity", 4.0f);

	pixelShaderWithNormals->SetFloat3("environmentAmbient", DirectX::XMFLOAT3(0.2f, 0.2f, 0.2f));
	pixelShaderWithNormals->SetFloat("specularIntensity", 2.0f);

	// Actually draw the meshes
	renderer->DrawMeshes(context, samplerState, entities, camera);

	// Present the back buffer to the user
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain, the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}

// --------------------------------------------------------
// Loads six individual textures (the six facesof a cube map), then
// creates a blank cube map and copies each of the six textures to
// another face.  Afterwards, creates a shader resource view for
// the cube map and cleans up all of the temporary resources.
// --------------------------------------------------------
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Game::CreateCubemap(
	const wchar_t* right,
	const wchar_t* left,
	const wchar_t* up,
	const wchar_t* down,
	const wchar_t* front,
	const wchar_t* back
)
{
	// Load the textures in an array
	ID3D11Texture2D* textures[6] = {};
	CreateWICTextureFromFile(device.Get(), right, (ID3D11Resource**)&textures[0], 0);
	CreateWICTextureFromFile(device.Get(), left, (ID3D11Resource**)&textures[1], 0);
	CreateWICTextureFromFile(device.Get(), up, (ID3D11Resource**)&textures[2], 0);
	CreateWICTextureFromFile(device.Get(), down, (ID3D11Resource**)&textures[3], 0);
	CreateWICTextureFromFile(device.Get(), front, (ID3D11Resource**)&textures[4], 0);
	CreateWICTextureFromFile(device.Get(), back, (ID3D11Resource**)&textures[5], 0);

	// We'll assume all of the textures are the same color format and resolution,
	// so get the description of the first shader resource view
	D3D11_TEXTURE2D_DESC faceDesc = {};
	textures[0]->GetDesc(&faceDesc);
	
	// Describe the resource for the cube map, which is simply
	// a "texture 2d array". This is a special GPU resource format,
	// NOT just a C++ array of textures!!!
	D3D11_TEXTURE2D_DESC cubeDesc = {};
	cubeDesc.ArraySize = 6; // Cube map!
	cubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // We'll be using as a texture in a shader
	cubeDesc.CPUAccessFlags = 0; // No read back
	cubeDesc.Format = faceDesc.Format; // Match the loaded texture's color format
	cubeDesc.Width = faceDesc.Width; // Match the size
	cubeDesc.Height = faceDesc.Height; // Match the size
	cubeDesc.MipLevels = 1; // Only need 1
	cubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; // A CUBE MAP, not 6 separate textures
	cubeDesc.Usage = D3D11_USAGE_DEFAULT; // Standard usage
	cubeDesc.SampleDesc.Count = 1;
	cubeDesc.SampleDesc.Quality = 0;
	
	// Create the actual texture resource
	ID3D11Texture2D* cubeMapTexture = 0;
	device->CreateTexture2D(&cubeDesc, 0, &cubeMapTexture);
	
	// Loop through the individual face textures and copy them,
	// one at a time, to the cube map texure
	for (int i = 0; i < 6; i++)
	{
		// Calculate the subresource position to copy into
		unsigned int subresource = D3D11CalcSubresource(
			0, // Which mip (zero, since there's only one)
			i, // Which array element?
			1); // How many mip levels are in the texture?
			// Copy from one resource (texture) to another
		context->CopySubresourceRegion(
			cubeMapTexture, // Destination resource
			subresource, // Dest subresource index (one of the array elements)
			0, 0, 0, // XYZ location of copy
			textures[i], // Source resource
			0, // Source subresource index (we're assuming there's only one)
			0); // Source subresource "box" of data to copy (zero means the whole thing)
	}
	// At this point, all of the faces have been copied into the
	// cube map texture, so we can describe a shader resource view for it
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubeDesc.Format; // Same format as texture
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; // Treat this as a cube!
	srvDesc.TextureCube.MipLevels = 1; // Only need access to 1 mip
	srvDesc.TextureCube.MostDetailedMip = 0; // Index of the first mip we want to see
	// Make the SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV;
	device->CreateShaderResourceView(cubeMapTexture, &srvDesc, cubeSRV.GetAddressOf());
	// Now that we're done, clean up the stuff we don't need anymore
	cubeMapTexture->Release(); // Done with this particular reference (the SRV has another)
	for (int i = 0; i < 6; i++)
		textures[i]->Release();
	// Send back the SRV, which is what we need for our shaders
	return cubeSRV;
};