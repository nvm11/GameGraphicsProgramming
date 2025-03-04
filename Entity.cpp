#include "Entity.h"
#include "Graphics.h"

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
	std::shared_ptr<SimpleVertexShader> vs = material->GetVertexShader();
	std::shared_ptr<SimplePixelShader> ps = material->GetPixelShader();
	//vertex shader
	vs->SetMatrix4x4("world", transform.GetWorldMatrix()); // match variable
	vs->SetMatrix4x4("view", activeCam->GetView()); // names in your
	vs->SetMatrix4x4("projection", activeCam->GetProjection()); // shader’s cbuffer!
	vs->CopyAllBufferData();
	ps->SetFloat3("colorTint", material->GetColor());
	ps->CopyAllBufferData();

	//set (activate) shaders for the entity
	material->GetVertexShader()->SetShader();
	material->GetPixelShader()->SetShader();

	//Responsible for:
	//-Setting correct Vertex and Index Buffers
	//-Telling D3D to render with bound resources
	mesh->Draw();
}
