#pragma once
#include <wrl/client.h>
#include <d3d11.h>
class Mesh
{
private:
	//Data--

	//Buffer Pointers-
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer; //vertex buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer; //index buffer

	//Data Counts-
	int indices; //index count
	int vertices; //vertex count

public:
	//Constructor--
	//Creates buffers and 
	Mesh(int indices, int vertices);

	//Destructor--
	//most likely empty, still necessary so ComPtrs clean up
	~Mesh();

	//Methods--
	
	//Getters-
	//Returns the vertex buffer ComPtr
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() const;
	//Returns index buffer ComPtr
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer() const;
	//Returns number of indices
	int GetIndexCount() const;
	//Returns number of vertices
	int GetVertexCount() const;

	//Output-
	//Sets buffers and draws using indices count
	void Draw();
};

