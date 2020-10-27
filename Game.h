#pragma once

#include "DXCore.h"
#include "Mesh.h"
#include "Material.h"
#include "Renderer.h"
#include "Camera.h"
#include "Lights.h"
#include "Skybox.h"

#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <vector>
#include <memory>

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Generates a cubemap from a set of path strings
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back
	);

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateBasicGeometry();

	// Shaders and shader-related constructs
	SimplePixelShader* pixelShader;
	SimplePixelShader* pixelShaderWithNormals;
	SimpleVertexShader* vertexShader;
	SimpleVertexShader* vertexShaderWithNormals;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	// Custom renderer
	Renderer* renderer;

	// Camera
	Camera* camera;

	// Entities and Materials
	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
	std::vector<Entity> entities;

	// Texture info 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cushionDiffuseMap;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cushionNormalMap;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rockDiffuseMap;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rockNormalMap;

	// Sampler info
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	// Skybox info
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMap;
	SimpleVertexShader* vertexShaderSkybox;
	SimplePixelShader* pixelShaderSkybox;
	Skybox* skybox;
};

