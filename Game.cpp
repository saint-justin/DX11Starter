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
	camera = new Camera(0, 0, -40, (float)this->width / this->height);
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

	// Delete al the meshes
	for (int i = 0; i < meshes.size(); i++) {
		delete meshes[i];
	}

	// Delete all the materials
	for (int i = 0; i < materials.size(); i++) {
		delete materials[i];
	}

	// Clean up ImGui's memory
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init(HWND hWnd)
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

	// Setting up IMGUI
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	showGui = true;
	ImGui_ImplWin32_Init(hWnd); // Unclear on if I need this?
	ImGui_ImplDX11_Init(device.Get(), context.Get());
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
	Material* cushionNoNormal = new Material(white, pixelShader, vertexShader, cushionDiffuseMap, 1);
	Material* cushionNormal = new Material(white, pixelShaderWithNormals, vertexShaderWithNormals, cushionDiffuseMap, cushionNormalMap, 2);
	Material* rocksNoNormal = new Material(white, pixelShader, vertexShader, rockDiffuseMap, 3);
	Material* rocksNormal = new Material(white, pixelShaderWithNormals, vertexShaderWithNormals, rockDiffuseMap, rockNormalMap, 4);

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

	// Spawn in entities with random meshes, materials, and locations
	AddGeo(50);
}

// Adds 10 more geometry items to the scene
void Game::AddGeo(int n)
{
	// Spawn in entities with random meshes, materials, and locations
	for (int i = 0; i < n; i++) {
		AddSingleGeo();
	}

	renderer->SetDirty();
}

// Adds a single random piece of geometry to the scene
void Game::AddSingleGeo() {
	Entity e = Entity(meshes[rand() % meshes.size()], materials[rand() % materials.size()]);

	float xPos = (rand() % 30) - 15.0f;
	float yPos = (rand() % 30) - 15.0f;
	float zPos = (rand() % 30) - 15.0f;
	e.GetTransform()->SetPosition(xPos, yPos, zPos);

	float xRot = rand() % 360;
	float yRot = rand() % 360;
	float zRot = rand() % 360;
	e.GetTransform()->SetRotation(xRot, yRot, zRot);

	entities.push_back(e);
}

// Removes 10 pieces of geometry from the scene
void Game::RemoveGeo(int n)
{
	for (int i = 0; i < n; i++) {
		if (entities.size() == 0) return;
		entities.pop_back();
	}

	renderer->SetDirty();
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

	if (GetAsyncKeyState('Q')) RemoveGeo(10);
	else if (GetAsyncKeyState('E')) AddGeo(10);

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

	// Check if the render queue needs to be cleaned
	if (renderer->GetDirty())
		renderer->GenerateRenderQueue(entities);

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
	if (drawWithRenderQueue)
		renderer->DrawMeshesQueued(context, samplerState, camera);
	else
		renderer->DrawMeshes(context, samplerState, entities, camera);


	// Draw the GUI to the screen
	DrawGui();

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

void Game::DrawGui() {
	// Setup
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Adding/Removing objects
	ImGui::Begin("Helpful Stats");
	if (ImGui::Button("-100 objects"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		RemoveGeo(100);
	ImGui::SameLine();
	if (ImGui::Button("+100 objects"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		AddGeo(100);
	ImGui::Text("You can also add or remove 10 shapes at a \ntime via the Q and E keys respectively.");

	// Toggle for the render queue
	ImGui::Checkbox("Use Render Queue?", &drawWithRenderQueue);

	// Showing FPS
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Total objects being rendered: %i", entities.size());

	// Actually displaying
	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}