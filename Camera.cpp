#include "Camera.h"
using namespace DirectX;

Camera::Camera(XMFLOAT3 initialPos, float fov, float aspectRatio, float nearClipDist, float farClipDist, bool perspective)
	:fovAngle(fov), nearClipDist(nearClipDist), farClipDist(farClipDist), isPerspective(perspective)
{
	transform.SetPosition(initialPos);
	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

Camera::~Camera()
{
	//Memory cleanup if needed
}

XMFLOAT4X4 Camera::GetView()
{
	return view;
}

XMFLOAT4X4 Camera::GetProjection()
{
	return projection;
}

void Camera::UpdateProjectionMatrix(float aspect)
{
	//create temp math-type matix
	XMMATRIX projMatrix;

	//check projection type
	if (isPerspective) {
		projMatrix = XMMatrixPerspectiveFovLH(fovAngle,
			aspect,
			nearClipDist,
			farClipDist);
	}
	else {

	}

	//store result
}

void Camera::UpdateViewMatrix()
{
}

void Camera::Update(float dt)
{
}
