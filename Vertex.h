#pragma once

#include <DirectXMath.h>

// --------------------------------------------------------
// A custom vertex definition
//
// You will eventually ADD TO this, and/or make more of these!
// --------------------------------------------------------
struct Vertex
{
	DirectX::XMFLOAT3 Position;	    // The local position of the vertex
	DirectX::XMFLOAT2 UV;			// UV texture coordinates
	DirectX::XMFLOAT3 Normal;	    // Surface normals (used for lighting)
	DirectX::XMFLOAT3 Tangent;		// Tangent used for normal mapping
};