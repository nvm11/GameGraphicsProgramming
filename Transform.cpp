#include "Transform.h"

Transform::Transform()
{
}

Transform::~Transform()
{
}

void Transform::SetPosition(float x, float y, float z)
{
}

void Transform::SetPosition(DirectX::XMFLOAT3 newPos)
{
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
}

void Transform::SetRotation(DirectX::XMFLOAT3 newRot)
{
}

void Transform::SetScale(float x, float y, float z)
{
}

void Transform::SetScale(DirectX::XMFLOAT3 newScale)
{
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Transform::GetRotation()
{
	return pitchYawRoll;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	return world;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	return worldInverseTranspose;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 newPos)
{
}

void Transform::Rotate(float x, float y, float z)
{
}

void Transform::Rotate(DirectX::XMFLOAT3 newRot)
{
}

void Transform::Scale(float x, float y, float z)
{
}

void Transform::Scale(DirectX::XMFLOAT3 newScale)
{
}
