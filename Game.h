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
#include "Sky.h"

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
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; //black

	//light objects
	std::vector<Lights> lights;
	//direction lights
	Lights directionLight1 = {}; //zero struct
	//Lights directionLight2 = {};
	//spot light
	Lights spotLight1 = {};
	Lights spotLight2 = {};
	//point lights
	//Lights pointLight1 = {};
	//Lights pointLight2 = {};

	//lighting values
	XMFLOAT3 ambientLight = XMFLOAT3(0.25f, 0.25f, 0.25f); //complement gray "sky"

	//sky
	std::shared_ptr<Sky> defaultSky;


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

	//variable for floor
	std::shared_ptr<Entity> floor;

	//Shadows
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;
	DirectX::XMFLOAT4X4 lightViewMatrix;
	DirectX::XMFLOAT4X4 lightProjectionMatrix;
	unsigned int shadowMapResolution;

	std::shared_ptr<SimpleVertexShader> shadowMapVS;
	std::shared_ptr<SimplePixelShader> shadowMapPS;

	float lightProjectionSize;

	//Post Processing
	//resources that are shared among all post processes
	Microsoft::WRL::ComPtr<ID3D11SamplerState> ppSampler;
	std::shared_ptr<SimpleVertexShader> ppVS;
	//resources that are tied to a particular post process
	std::shared_ptr<SimplePixelShader> ppPS;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> ppRTV; //rendering
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ppSRV; //sampling
	int blurRadius = 0;
	float parallaxScale = 0.001f;
	int parallaxSamples = 5;

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void CreateGeometry();

	//UI helper functions
	void UpdateUI(float deltaTime);
	void DrawUI();

	void CreateShadowMap();
	void DrawShadowMap();

	void ResetPostProcess();
	void PreparePostProcess();
};

