#pragma once
#include <memory>
#include <wrl/client.h>
#include <DirectXMath.h>

#include "Transform.h"
#include "Mesh.h"
#include "BufferStructs.h"
#include "Camera.h"

class Entity
{
private:
	//Fields
	Transform transform;
	std::shared_ptr<Mesh> mesh;

	//zero out struct
	//here temporarily
	ShaderData vsData = {};

public:
	//Constructors
	Entity(const std::shared_ptr<Mesh> mesh);
	~Entity();

	//Methods
	//Getters
	std::shared_ptr<Mesh> GetMesh();
	Transform& GetTransform();
	//Drawing
	void Draw(std::shared_ptr<Camera> activeCam);
};

