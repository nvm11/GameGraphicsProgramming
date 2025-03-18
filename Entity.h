#pragma once
#include <memory>
#include <wrl/client.h>
#include <DirectXMath.h>

#include "Transform.h"
#include "Mesh.h"
//#include "BufferStructs.h"
#include "Camera.h"
#include "Material.h"

class Entity
{
private:
	//Fields
	Transform transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;

public:
	//Constructors
	Entity(const std::shared_ptr<Mesh> mesh, const std::shared_ptr<Material> material);
	~Entity();

	//Methods
	//Getters
	std::shared_ptr<Mesh> GetMesh();
	Transform& GetTransform();
	std::shared_ptr<Material> GetMaterial();
	//Setters
	void SetMaterial(std::shared_ptr<Material> newMat);

	//Drawing
	void Draw(std::shared_ptr<Camera> activeCam, const DirectX::XMFLOAT2 uvScale = DirectX::XMFLOAT2(1, 1), const DirectX::XMFLOAT2 uvOffset = DirectX::XMFLOAT2(0, 0));
};

