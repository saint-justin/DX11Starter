#pragma once
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"

class Entity
{
public:
	// Initializes mesh w/ a pointer to a base mesh
	Entity(Mesh* _mesh);
	
	// Getters
	Mesh* GetMesh();
	Transform* GetTransform();

private:
	Mesh* mesh;
	Transform transform;
};

