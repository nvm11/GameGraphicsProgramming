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
	//create xmvector and store result in original position
	XMStoreFloat3(&position, XMLoadFloat3(&newPos));
	dirty = true; //matrix needs rebuild
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	//call method overload with xmfloat compatibility
	SetRotation(XMFLOAT3(pitch, yaw, roll));
}

void Transform::SetRotation(DirectX::XMFLOAT3 newRot)
{
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

void Transform::MoveAbsolute(float x, float y, float z)
{
	MoveAbsolute(XMFLOAT3(x, y, z));
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 newPos)
{
	//convert both positions to XMVectors.
	//combine positions
	//store new data in the original XMFloat3
	XMStoreFloat3(&position,
		XMVectorAdd(XMLoadFloat3(&position)
			, XMLoadFloat3(&newPos)));

	dirty = true;
}

void Transform::Rotate(float x, float y, float z)
{
	Rotate(XMFLOAT3(x, y, z));
}

void Transform::Rotate(DirectX::XMFLOAT3 newRot)
{
	XMStoreFloat3(&pitchYawRoll,
		XMVectorAdd(XMLoadFloat3(&pitchYawRoll),
			XMLoadFloat3(&newRot)));
}

void Transform::Scale(float x, float y, float z)
{
	Scale(XMFLOAT3(x, y, z));
}

void Transform::Scale(DirectX::XMFLOAT3 newScale)
{
	XMStoreFloat3(&scale,
		XMVectorMultiply(XMLoadFloat3(&scale),
			XMLoadFloat3(&newScale)));
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (dirty) {
		//Remake matrix
		RemakeMatrices();
	}

	return world;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	if (dirty) {
		RemakeMatrices();
	}

	return worldInverseTranspose;
}

void Transform::RemakeMatrices() {
	//convert data to matrices
	XMMATRIX positionMatrix = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
	XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);

	//create new SIMD world matrix: S * R * T
	XMMATRIX worldMatrix = XMMatrixMultiply(XMMatrixMultiply(
		scaleMatrix,
		rotationMatrix),
		positionMatrix);

	//store results in storage types
	XMStoreFloat4x4(&world, worldMatrix);
	XMStoreFloat4x4(&worldInverseTranspose, XMMatrixInverse(0, XMMatrixTranspose(worldMatrix)));

	//tell class matrices are up to date
	dirty = false;
}
