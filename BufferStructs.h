#pragma once
#include "DirectXMath.h"

struct ShaderData {
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT4X4 offset;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};