#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

#include <memory>
#include <vector>

#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "Material.h"
//contains constant buffer structs
//Removed with Simple Shader
//#include "BufferStructs.h"
//contains simplified shaders functionality
#include "SimpleShader.h"
#include "Lights.h"

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
	float color[4] = { 0.5f, 0.5f, 0.5f, 0.0f }; //Gray
	
	//light objects
	std::vector<Lights> lights;
	Lights directionLight1 = {}; //zero struct
	//lighting values
	XMFLOAT3 ambientLight = XMFLOAT3(0.25f, 0.25f, 0.25f); //complement gray "sky"


	////Vertices Default Colors
	//XMFLOAT4 top;
	//XMFLOAT4 left;
	//XMFLOAT4 right;

	////Vertices Default Positions
	//// Vertex positions for triangle
	//XMFLOAT3 topPosition;
	//XMFLOAT3 leftPosition;
	//XMFLOAT3 rightPosition;


	//container for cameras
	std::vector<std::shared_ptr<Camera>> cams;
	int activeCam; //index of active camera

	//container for meshes
	std::vector<std::shared_ptr<Mesh>> meshes;
	//container for entities
	std::vector<std::shared_ptr<Entity>> entities;

	// Initialization helper methods - feel free to customize, combine, remove, etc.
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
};

