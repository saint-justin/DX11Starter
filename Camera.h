#pragma once
#include <Windows.h>
#include "Transform.h"
#include <DirectXMath.h>

class Camera
{
public:
	Camera(float x, float y, float z, float aspectRatio);
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float deltaTime, HWND windowHandle);

private:
	Transform transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;

	POINT mousePos;

	/*
	Items that can be added in later for further customization

	float fieldOfView;
	float nearClipPlane;
	float farClipPlane;
	float moveSpeed;
	float lookSpeed;
	*/

};

