#include "Entity.h"

Entity::Entity(const std::shared_ptr<Mesh> mesh)
{
}

Entity::~Entity()
{
}

std::shared_ptr<Mesh> Entity::GetMesh()
{
    return std::shared_ptr<Mesh>();
}

Transform& Entity::GetTransform()
{
    // TODO: insert return statement here
}

void Entity::Draw()
{
}
