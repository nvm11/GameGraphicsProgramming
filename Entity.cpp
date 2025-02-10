#include "Entity.h"

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

void Entity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer)
{

}
