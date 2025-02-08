#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

#include <memory>
#include <vector>

#include "Mesh.h"
//contains constant buffer structs
#include "BufferStructs.h"

using namespace DirectX;

class Game
{
public:
	// Basic OOP setup
	Game() = default;
	~Game();
	Game(const Game&) = delete; // Remove copy constructor
	Game& operator=(const Game&) = delete; // Remove copy-assignment operator

	// Primary functions
	void Initialize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void OnResize();

private:
	//UI related fields
	bool showDemoUI = false; //only draw demo text if needed
	//Local array for storing value between frames
	float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f }; //Cornflower blue

	////Vertices Default Colors
	//XMFLOAT4 top;
	//XMFLOAT4 left;
	//XMFLOAT4 right;

	////Vertices Default Positions
	//// Vertex positions for triangle
	//XMFLOAT3 topPosition;
	//XMFLOAT3 leftPosition;
	//XMFLOAT3 rightPosition;

	//Instances of Mesh class
	std::shared_ptr<Mesh> starterMesh;
	std::shared_ptr<Mesh> secondMesh;
	std::shared_ptr<Mesh> thirdMesh;

	//container for meshes
	std::vector<std::shared_ptr<Mesh>> meshes;




	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	void CreateGeometry();

	//UI helper functions
	void UpdateUI(float deltaTime);
	void DrawUI();

	//Reset color and position values
	void ResetVertices();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	//create vertex data struct
	ShaderData vsData = {};
};

