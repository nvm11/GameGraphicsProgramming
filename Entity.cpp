#include "Entity.h"
#include "Graphics.h"
#include "Window.h"

Entity::Entity(const std::shared_ptr<Mesh> mesh, const std::shared_ptr<Material> material)
	: mesh(mesh), material(material)
{
	//Transform field is a reference, 
	//it is created when Entity is instantiated
}

Entity::~Entity()
{

}

std::shared_ptr<Mesh> Entity::GetMesh()
{
	return mesh;
}

Transform& Entity::GetTransform()
{
	return transform;
}

std::shared_ptr<Material> Entity::GetMaterial()
{
	return material;
}

void Entity::SetMaterial(std::shared_ptr<Material> newMat)
{
	material = newMat;
}

void Entity::Draw(std::shared_ptr<Camera> activeCam)
{
	material->PrepareMaterial(transform, activeCam);

	//Responsible for:
	//-Setting correct Vertex and Index Buffers
	//-Telling D3D to render with bound resources
	mesh->Draw();
}
