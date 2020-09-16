#pragma once
#include <DirectXMath.h>

class Transform
{
public:
	// Constructor for initialization
	Transform();

	// Collection of setters
	void SetPosition(float x, float y, float z);
	void SetRotation(float pitch, float yaw, float roll);
	void SetScale(float x, float y, float z);

	// Collection of getters
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetPitchYawRoll();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	bool GetMatrixDirty();

	// Collection of transformers that adjust transformation values
	void MoveAbsolute(float x, float y, float z);
	void Rotate(float pitch, float yaw, float roll);
	void Scale(float x, float y, float z);

	// Collection of world matrix functions
	void updateWorldMatrix();

private:
	// World matrix for projections
	DirectX::XMFLOAT4X4 worldMatrix;

	// Position, rotation, and scale for object tracking
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;		// Note: Could be updated to quaternions for more robust rotations in the future

	// Boolean to track if we need to rebuild the world matrix before rendering
	bool matrixDirty;
};

