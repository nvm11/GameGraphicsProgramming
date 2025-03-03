#include "Entity.h"
#include "Graphics.h"

Entity::Entity(const std::shared_ptr<Mesh> mesh)
	: mesh(mesh)
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

void Entity::Draw(std::shared_ptr<Camera> activeCam)
{
	//Set default for constant buffer
	vsData.colorTint = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vsData.offset = transform.GetWorldMatrix();
	vsData.view = activeCam->GetView();
	vsData.projection = activeCam->GetProjection();


	//Responsible for:
	//-Setting correct Vertex and Index Buffers
	//-Telling D3D to render with bound resources
	mesh->Draw();
}
