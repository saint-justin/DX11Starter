#include "Entity.h"

Entity::Entity(Mesh* _mesh, Material* _material)
{
	mesh = _mesh;
	material = _material;
	renderPriority = _material->renderPriority;
}

Mesh* Entity::GetMesh() { return mesh; };
Transform* Entity::GetTransform() { return &transform; }
Material* Entity::GetMaterial() { return material; }