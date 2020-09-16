#include "Transform.h"
// Initializes the entity's transform
Transform::Transform() 
{
	SetPosition(0.0f, 0.0f, 0.0f);
	SetRotation(0.0f, 0.0f, 0.0f);
	SetScale(1.0f, 1.0f, 1.0f);
	updateWorldMatrix();
}

// --------------------------------- Setters for position, scale, and rotation
// Setter for position
void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;

	matrixDirty = true;
}

// Setter for rotation
void Transform::SetRotation(float pitch, float yaw, float roll)
{
	rotation.x = pitch;
	rotation.y = yaw;
	rotation.z = roll;

	matrixDirty = true;
}

// Basic setter for scale
void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;

	matrixDirty = true;
}

// --------------------------------- Getters for internal members
DirectX::XMFLOAT3 Transform::GetPosition() { return position; }
DirectX::XMFLOAT3 Transform::GetPitchYawRoll() { return rotation; }
DirectX::XMFLOAT3 Transform::GetScale() { return scale; }
bool Transform::GetMatrixDirty() { return matrixDirty; }

// --------------------------------- Functions for transforming the entity's position, rotation, or scale
// Moves the entity according the absolute world position
void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;

	matrixDirty = true;
}

// Rotates the object about pitch, yaw, and roll axes
void Transform::Rotate(float pitch, float yaw, float roll) 
{
	rotation.x += pitch;
	rotation.y += yaw;
	rotation.z += roll;

	matrixDirty = true;
}

// Scales the object my multiplying current by passed values
void Transform::Scale(float x, float y, float z) 
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;

	matrixDirty = true;
}

// Generate and set in place an updated world matrix
void Transform::updateWorldMatrix()
{
	// Generating trans/rot/scale matrices
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(position.x, position.x, position.z);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	// Calculating and setting new world matrix
	DirectX::XMMATRIX world = translationMatrix * rotationMatrix * scaleMatrix;
	DirectX::XMStoreFloat4x4(&worldMatrix, world);

	// Denote that the matrix has been cleaned
	matrixDirty = false;
}

// Gets a cleaned world matrix when requested
DirectX::XMFLOAT4X4 Transform::GetWorldMatrix() 
{ 
	if (matrixDirty)
		updateWorldMatrix();

	return worldMatrix; 
}
