#include "Entity.h"

Entity::Entity(Mesh* _mesh) {
	mesh = _mesh;
}

Mesh* Entity::GetMesh() { return mesh; };
Transform* Entity::GetTransform() { return &transform; };