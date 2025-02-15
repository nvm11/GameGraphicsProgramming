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

void Entity::Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer, std::shared_ptr<Camera> activeCam)
{
	//Map, Memcpy, and Unmap cbuffer
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	Graphics::Context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
	Graphics::Context->Unmap(constantBuffer.Get(), 0);

	//Bind the constant buffer to the vertex shader
	Graphics::Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

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
