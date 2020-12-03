#include "Camera.h"
#include "stdio.h"

// Getters/Setters
DirectX::XMFLOAT4X4 Camera::GetViewMatrix() { return viewMatrix; }
DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix() { return projMatrix; }

// Initializes the camera
Camera::Camera(float x, float y, float z, float aspectRatio) 
{
	transform.SetPosition(x, y, z);
	UpdateProjectionMatrix(aspectRatio);
	UpdateViewMatrix();
	GetCursorPos(&mousePos);
}

// Updates the camera's projection matrix
void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	// Inputs are: Field of View, Aspect Ratio, Near Clip Plane, Far Clip Plane --> Parameterize these soon
	DirectX::XMMATRIX newProjMatrix =  DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI / 4.0f, aspectRatio, 0.01f, 1000.0f);
	DirectX::XMStoreFloat4x4(&projMatrix, newProjMatrix);
}

// Updates the camera's view matrix
void Camera::UpdateViewMatrix() 
{
	// Get rotation and direction values
	DirectX::XMFLOAT3 rot = transform.GetPitchYawRoll();
	DirectX::XMVECTOR dir = DirectX::XMVector3Rotate(
		DirectX::XMVectorSet(0, 0, 1, 0),	
		DirectX::XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rot)));

	// Generatee and set our view matrix
	DirectX::XMMATRIX view = DirectX::XMMatrixLookToLH(
		XMLoadFloat3(&transform.GetPosition()),
		dir,
		DirectX::XMVectorSet(0, 1, 0, 0));
	DirectX::XMStoreFloat4x4(&viewMatrix, view);
}

// Updates the camera once p/ frame 
void Camera::Update(float deltaTime, HWND windowHandle) 
{
	// Placeholder speeds
	float moveSpeed = 5.0f * deltaTime;
	float mouseLookSpeed = 50.0f * deltaTime;

	// Update positioning based on keyboard inputs W, A, S, D, X, & Spacebar
	if (GetAsyncKeyState('W') & 0x8000) { transform.MoveRelative(0, 0, moveSpeed); }			// Forward
	if (GetAsyncKeyState('S') & 0x8000) { transform.MoveRelative(0, 0, moveSpeed * -1.0f); }	// Backward
	if (GetAsyncKeyState('D') & 0x8000) { transform.MoveRelative(moveSpeed, 0, 0); }			// Right
	if (GetAsyncKeyState('A') & 0x8000) { transform.MoveRelative(moveSpeed * -1.0f, 0, 0); }	// Left
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) { transform.MoveAbsolute(0, moveSpeed, 0); }		// Up
	if (GetAsyncKeyState('X') & 0x8000) { transform.MoveAbsolute(0, moveSpeed * -1.0f, 0); }	// Down

	// Update mouse position if left mouse is held down 
	POINT newMousePos = {};
	GetCursorPos(&newMousePos);
	ScreenToClient(windowHandle, &newMousePos);

	// Check if the player's traing to actually move the camera
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
		float mouseDiffX = -(mousePos.x - newMousePos.x) * mouseLookSpeed * 0.01f;
		float mouseDiffY = -(mousePos.y - newMousePos.y) * mouseLookSpeed * 0.01f;

		// Adjust the camera based on mouse movement
		DirectX::XMFLOAT3 oldRot = transform.GetPitchYawRoll();
		transform.SetRotation(oldRot.x + mouseDiffY, oldRot.y + mouseDiffX, oldRot.z);
	}
	mousePos = newMousePos;

	// Update the view matrix to align with new positioning
	UpdateViewMatrix();
}