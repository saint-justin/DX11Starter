#pragma once
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"

class Entity
{
public:
	// Initializes mesh w/ a pointer to a base mesh
	Entity(Mesh* _mesh, Material* _material);
	
	// Getters
	Mesh* GetMesh();
	Transform* GetTransform();
	Material* GetMaterial();

private:
	Mesh* mesh;
	Material* material;
	Transform transform;
};

