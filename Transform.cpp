#include "Transform.h"
using namespace DirectX;

Transform::Transform() :
	position(0, 0, 0), scale(1, 1, 1), pitchYawRoll(0, 0, 0), dirty(false)
{
	//store matrices
	XMStoreFloat4x4(&world, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixIdentity());
}

Transform::~Transform()
{
	//Place any needed delete statements here
}

void Transform::SetPosition(float x, float y, float z)
{
	SetPosition(XMFLOAT3(x, y, z));
}

void Transform::SetPosition(DirectX::XMFLOAT3 newPos)
{
	XMStoreFloat3(&position, XMLoadFloat3(&newPos));
	dirty = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	//call method overload with xmfloat compatibility
	SetRotation(XMFLOAT3(pitch, yaw, roll));
}

void Transform::SetRotation(DirectX::XMFLOAT3 newRot)
{
	//create xmvector and store result in original position
	XMStoreFloat3(&pitchYawRoll, XMLoadFloat3(&newRot));
	dirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
	SetScale(XMFLOAT3(x, y, z));
}

void Transform::SetScale(DirectX::XMFLOAT3 newScale)
{
	XMStoreFloat3(&scale, XMLoadFloat3(&newScale));
	dirty = true;
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
