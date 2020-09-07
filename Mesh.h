#pragma once
#include <d3d11.h>
#include <wrl/client.h>

#include "Vertex.h"

class Mesh
{
public:
	// Setup
	Mesh(Vertex verts[], int numVerts, unsigned int indices[], int numIndices, Microsoft::WRL::ComPtr<ID3D11Device> device);

	// Vertex and Index buffers
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

	// Index count access
	int GetIndexCount();

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	int indexCount = 0;
};

