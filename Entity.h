#pragma once
#include <memory>

#include "Transform.h"
#include "Mesh.h"

class Entity
{
private:
	//Fields
	Transform transform;
	std::shared_ptr<Mesh> mesh;

public:
	//Constructors
	Entity(const std::shared_ptr<Mesh> mesh);
	~Entity();

	//Methods
	//Getters
	std::shared_ptr<Mesh> GetMesh();
	Transform& GetTransform();
	//Drawing
	void Draw();
};

