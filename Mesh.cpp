#include "Mesh.h"
#include "Vertex.h"
#include "Graphics.h"

Mesh::Mesh(int indices, int vertces) {
	//set fields from params
	this->indices = indices;
	this->vertices = vertces;

	//create vertices

	//create indices

	//create buffers
}

Mesh::~Mesh() {
	//If cleanup required, place code here
}

//Return whole ComPtr Objects
Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer() const {
	return vertexBuffer;
}
Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer() const {
	return indexBuffer;
}

//Return counts of vertices and indices
int Mesh::GetVertexCount() const {
	return vertices;
}

int Mesh::GetIndexCount() const {
	return indices;
}