#include "Mesh.h"

// Constructer, generattes vertex buffer & index buffer from variables
Mesh::Mesh(Vertex verts[], int numVerts, unsigned int indices[], int numIndices, Microsoft::WRL::ComPtr<ID3D11Device> device) 
{
	// Set the number of indices in place
	indexCount = numIndices;

	// Generating the vertex buffer description & vertex buffer from info passed --------------------------------
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * numVerts;       
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Tells DirectX this is a vertex buffer
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// Create the proper struct to hold the initial vertex data
	D3D11_SUBRESOURCE_DATA initialVertexData;
	initialVertexData.pSysMem = verts;

	// Actually create the buffer with the initial data
	device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());


	// Generating the index buffer description & index buffer from info passed --------------------------------
	// - The description is created on the stack because we only need
	//    it to create the buffer.  The description is then useless.
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(unsigned int) * numIndices;	// 3 = number of indices in the buffer
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;	// Tells DirectX this is an index buffer
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// Create the proper struct to hold the initial index data
	D3D11_SUBRESOURCE_DATA initialIndexData;
	initialIndexData.pSysMem = indices;

	// Actually create the buffer with the initial data
	device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
}

// Returns the ComPtr to the vertex buffer
Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer() 
{
	return vertexBuffer;
}

// Returns the ComPtr to the IndexBuffer
Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

// Returns the index count
int Mesh::GetIndexCount() 
{
	return indexCount;
}