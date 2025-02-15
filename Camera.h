#pragma once
#include "Transform.h"
#include "Input.h"

class Camera
{
private:
	//Fields
	//Transformations
	Transform transform;
	//Viewing
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	//Customization of viewing
	float fovAngle; //in radians
	float nearClipDist;
	float farClipDist;
	bool isPerspective;
};

