#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"

#include <DirectXMath.h>

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// This code assumes files are in "ImGui" subfolder!
// Adjust as necessary for your own folder structure and project setup
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

//Include DirectX Toolkit for textures
#include "WicTextureLoader.h"
//Include unordered map for adding t

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
	//Assign starting values for colors and vertices
	ResetVertices();

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	CreateGeometry();

	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Initialize ImGui itself & platform/renderer backends
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(Window::Handle());
		ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
		// Pick a style
		ImGui::StyleColorsDark();

		//Create cameras
		float aspectRatio = (float)Window::Width() / (float)Window::Height();

		//perspective
		std::shared_ptr<Camera> cam1 = std::make_shared<Camera>(
			XMFLOAT3(0.0f, 0.0f, -5.0f),
			XM_PIDIV4,
			aspectRatio,
			0.01f,
			100.0f);
		//orthographic
		std::shared_ptr<Camera> cam2 = std::make_shared<Camera>(
			XMFLOAT3(1.0f, 2.0f, -10.0f),
			XM_PIDIV2,
			aspectRatio,
			0.01f,
			100.0f,
			false);

		cams.push_back(cam1);
		cams.push_back(cam2);

		activeCam = 0;
	}
}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	//Create shaders
	//one texture, no normal map
	std::shared_ptr<SimpleVertexShader> basicVertexShader = std::make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> basicPixelShader = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"PixelShader.cso").c_str());

	//two textures, no normal map
	std::shared_ptr<SimplePixelShader> twoTexturePS = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"TwoTexturePS.cso").c_str());

	//one texture with normal map
	std::shared_ptr<SimpleVertexShader> normalMapVS = std::make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"NormalMapVS.cso").c_str());
	std::shared_ptr<SimplePixelShader> normalMapPS = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"NormalMapPS.cso").c_str());
	std::shared_ptr<SimplePixelShader> normalMapSkyPS = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"NormalMapSkyPS.cso").c_str()); //reflect sky on objects

	std::shared_ptr<SimplePixelShader> PBRPixelShader = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"PBRPixelShader.cso").c_str()); //physically based


	//Give data to lights
	//Direction
	directionLight1.type = LIGHT_TYPE_DIRECTIONAL;
	directionLight1.direction = XMFLOAT3(1.0f, 0.1f, 0.0f);
	directionLight1.color = XMFLOAT3(1.0f, 0.3f, 0.4f); //maroon
	directionLight1.intensity = 1.0f;

	//directionLight2.type = LIGHT_TYPE_DIRECTIONAL;
	//directionLight2.direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	//directionLight2.color = XMFLOAT3(0.0f, 1.0f, 0.0f); //green
	//directionLight2.intensity = 1.1f;

	//Point
	//pointLight1.type = LIGHT_TYPE_POINT;
	//pointLight1.position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//pointLight1.color = XMFLOAT3(1.0f, 1.0f, 1.0f); //white
	//pointLight1.range = 10;
	//pointLight1.intensity = 2;

	//pointLight2.type = LIGHT_TYPE_POINT;
	//pointLight1.position = XMFLOAT3(6.0f, 1.0f, 0.0f);
	//pointLight2.color = XMFLOAT3(1.0f, 1.0f, 0.0f); //yellow
	//pointLight2.range = 15;
	//pointLight2.intensity = 1;

	//Spot
	spotLight1.type = LIGHT_TYPE_SPOT;
	spotLight1.position = XMFLOAT3(6.0f, 1.5f, 0.0f);
	spotLight1.direction = XMFLOAT3(0.0f, -1.0f, 0.0f); //straight down
	spotLight1.color = XMFLOAT3(1.0f, 0.0f, 0.0f); //red
	spotLight1.range = 5.0f;
	spotLight1.intensity = 2.0f;
	spotLight1.spotInnerAngle = XMConvertToRadians(30);
	spotLight1.spotOuterAngle = XMConvertToRadians(60);

	spotLight2.type = LIGHT_TYPE_SPOT;
	spotLight2.position = XMFLOAT3(9.0f, 1.0f, 0.0f);
	spotLight2.direction = XMFLOAT3(0.0f, -0.8f, 0.0f); //straight down
	spotLight2.color = XMFLOAT3(0.0f, 0.0f, 1.0f); //blue
	spotLight2.range = 10.0f;
	spotLight2.intensity = 1.0f;
	spotLight2.spotInnerAngle = XMConvertToRadians(20);
	spotLight2.spotOuterAngle = XMConvertToRadians(40);

	//Add lights to vector
	lights.insert(lights.end(), { directionLight1, spotLight1, spotLight2 });

	//Load textures
	//create SRVs for textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeNormalsSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeMetalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeRoughnessSRV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedNormalsSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedMetalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedRoughnessSRV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>paintSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>paintNormalsSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>paintMetalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>paintRoughnessSRV;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>roughSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>roughNormalsSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>roughMetalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>roughRoughnessSRV;

	//Load textures (albedo)
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/bronze_albedo.png").c_str(), 0, bronzeSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/scratched_albedo.png").c_str(), 0, scratchedSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/paint_albedo.png").c_str(), 0, paintSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/rough_albedo.png").c_str(), 0, roughSRV.GetAddressOf());
	//Load normal maps
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/bronze_normals.png").c_str(), 0, bronzeNormalsSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/scratched_normals.png").c_str(), 0, scratchedNormalsSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/rough_normals.png").c_str(), 0, roughNormalsSRV.GetAddressOf());
	//Load metalness
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/bronze_metal.png").c_str(), 0, bronzeMetalSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/scratched_metal.png").c_str(), 0, scratchedMetalSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/rough_metal.png").c_str(), 0, roughMetalSRV.GetAddressOf());
	//Load roughness
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/bronze_roughness.png").c_str(), 0, bronzeRoughnessSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/scratched_roughness.png").c_str(), 0, scratchedRoughnessSRV.GetAddressOf());
	CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(), FixPath(L"../../Assets/Textures/rough_roughness.png").c_str(), 0, roughRoughnessSRV.GetAddressOf());

	//Define Sampler State
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampleState;
	//Create description
	D3D11_SAMPLER_DESC sampleDesc = {};
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; //how to handle outside 0-1
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.Filter = D3D11_FILTER_ANISOTROPIC; //"best" option
	sampleDesc.MaxAnisotropy = 16; //set max range for filter
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX; //minmap at all ranges

	Graphics::Device->CreateSamplerState(&sampleDesc, sampleState.GetAddressOf()); //set sample state

	//create pointers to meshes
	std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>(FixPath("../../Assets/Models/cube.obj").c_str());
	std::shared_ptr<Mesh> cylinderMesh = std::make_shared<Mesh>(FixPath("../../Assets/Models/cylinder.obj").c_str());
	std::shared_ptr<Mesh> helixMesh = std::make_shared<Mesh>(FixPath("../../Assets/Models/helix.obj").c_str());
	std::shared_ptr<Mesh> sphereMesh = std::make_shared<Mesh>(FixPath("../../Assets/Models/sphere.obj").c_str());
	std::shared_ptr<Mesh> torusMesh = std::make_shared<Mesh>(FixPath("../../Assets/Models/torus.obj").c_str());
	std::shared_ptr<Mesh> quadMesh = std::make_shared<Mesh>(FixPath("../../Assets/Models/quad.obj").c_str());
	std::shared_ptr<Mesh> quad2sidedMesh = std::make_shared<Mesh>(FixPath("../../Assets/Models/quad_double_sided.obj").c_str());


	//create sky
	defaultSky = std::make_shared<Sky>(cubeMesh, sampleState,
		FixPath(L"../../Assets/Skyboxes/Pink/right.png").c_str(),
		FixPath(L"../../Assets/Skyboxes/Pink/left.png").c_str(),
		FixPath(L"../../Assets/Skyboxes/Pink/up.png").c_str(),
		FixPath(L"../../Assets/Skyboxes/Pink/down.png").c_str(),
		FixPath(L"../../Assets/Skyboxes/Pink/front.png").c_str(),
		FixPath(L"../../Assets/Skyboxes/Pink/back.png").c_str());

	//add all meshes to vector
	meshes.insert(meshes.end(), { cubeMesh, cylinderMesh, helixMesh, sphereMesh, torusMesh, quadMesh, quad2sidedMesh });

	std::shared_ptr<Material> matBronzePBR = std::make_shared<Material>(normalMapVS, PBRPixelShader, XMFLOAT3(1, 1, 1));
	std::shared_ptr<Material> matScratchedPBR = std::make_shared<Material>(normalMapVS, PBRPixelShader, XMFLOAT3(1, 1, 1));
	std::shared_ptr<Material> matPaintPBR = std::make_shared<Material>(normalMapVS, PBRPixelShader, XMFLOAT3(1, 1, 1));
	std::shared_ptr<Material> matRoughPBR = std::make_shared<Material>(normalMapVS, PBRPixelShader, XMFLOAT3(1, 1, 1));

	//add samplers to materials
	matBronzePBR->AddSampler("BasicSampler", sampleState);
	matBronzePBR->AddTextureSRV("Albedo", bronzeSRV);
	matBronzePBR->AddTextureSRV("NormalMap", bronzeNormalsSRV);
	matBronzePBR->AddTextureSRV("RoughnessMap", bronzeRoughnessSRV);
	matBronzePBR->AddTextureSRV("MetalnessMap", bronzeMetalSRV);

	//do it again for scratched metal
	matScratchedPBR->AddSampler("BasicSampler", sampleState);
	matScratchedPBR->AddTextureSRV("Albedo", scratchedSRV);
	matScratchedPBR->AddTextureSRV("NormalMap", scratchedNormalsSRV);
	matScratchedPBR->AddTextureSRV("RoughnessMap", scratchedRoughnessSRV);
	matScratchedPBR->AddTextureSRV("MetalnessMap", scratchedMetalSRV);

	matPaintPBR->AddSampler("BasicSampler", sampleState);
	matPaintPBR->AddTextureSRV("Albedo", paintSRV);
	matPaintPBR->AddTextureSRV("NormalMap", paintNormalsSRV);
	matPaintPBR->AddTextureSRV("RoughnessMap", paintRoughnessSRV);
	matPaintPBR->AddTextureSRV("MetalnessMap", paintMetalSRV);

	matRoughPBR->AddSampler("BasicSampler", sampleState);
	matRoughPBR->AddTextureSRV("Albedo", roughSRV);
	matRoughPBR->AddTextureSRV("NormalMap", roughNormalsSRV);
	matRoughPBR->AddTextureSRV("RoughnessMap", roughRoughnessSRV);
	matRoughPBR->AddTextureSRV("MetalnessMap", roughMetalSRV);

	//create initial entities
	entities.push_back(std::make_shared<Entity>(meshes[0], matBronzePBR));
	entities.push_back(std::make_shared<Entity>(meshes[1], matBronzePBR));
	entities.push_back(std::make_shared<Entity>(meshes[2], matBronzePBR));
	entities.push_back(std::make_shared<Entity>(meshes[3], matBronzePBR));
	entities.push_back(std::make_shared<Entity>(meshes[4], matBronzePBR));
	entities.push_back(std::make_shared<Entity>(meshes[5], matBronzePBR));
	entities.push_back(std::make_shared<Entity>(meshes[6], matBronzePBR));
	//move them for spacing
	entities[0]->GetTransform().MoveAbsolute(-9, 0, 0);
	entities[1]->GetTransform().MoveAbsolute(-6, 0, 0);
	entities[2]->GetTransform().MoveAbsolute(-3, 0, 0);
	entities[3]->GetTransform().MoveAbsolute(0, 0, 0);
	entities[4]->GetTransform().MoveAbsolute(3, 0, 0);
	entities[5]->GetTransform().MoveAbsolute(6, 0, 0);
	entities[6]->GetTransform().MoveAbsolute(9, 0, 0);

	//create floor
	floor = std::make_shared<Entity>(quadMesh, matPaintPBR);
	//scale and move it
	floor->GetTransform().SetScale(XMFLOAT3(15.0f, 1.0f, 15.0f));
	floor->GetTransform().MoveAbsolute(XMFLOAT3(0.0f, -2.0f, 0.0f));

	//store size of entities
	size_t count = entities.size();
	//create more!
	for (size_t i = 0; i < count; i++)
	{
		//get mesh of entity
		std::shared_ptr<Mesh> mesh = entities[i]->GetMesh();
		//create new entities with normal and uv materials
		std::shared_ptr<Entity> normalMapSky = std::make_shared<Entity>(mesh, matScratchedPBR);
		std::shared_ptr<Entity> noNormalMap = std::make_shared<Entity>(mesh, matRoughPBR);
		//move horizontal
		normalMapSky->GetTransform().MoveAbsolute(entities[i]->GetTransform().GetPosition());
		noNormalMap->GetTransform().MoveAbsolute(entities[i]->GetTransform().GetPosition());
		//move vertical
		normalMapSky->GetTransform().MoveAbsolute(0, 3, 0);
		noNormalMap->GetTransform().MoveAbsolute(0, 6, 0);
		//add them to entities
		entities.push_back(normalMapSky);
		entities.push_back(noNormalMap);
	}
}



// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	//calc new aspect ratio
	float aspectRatio = (float)Window::Width() / (float)Window::Height();

	//Do a null check for startup
	for (size_t i = 0; i < cams.size(); i++) {
		cams[i]->UpdateProjectionMatrix(aspectRatio);
	}
}

void Game::UpdateUI(float deltaTime)
{
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);
	// Show the demo window?
	if (showDemoUI)
	{
		ImGui::ShowDemoWindow();
	}
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Must be done at very beginning so UI is up to date
	UpdateUI(deltaTime);

	//Only update active camera
	cams[activeCam]->Update(deltaTime);

	//rotate all entities
	for (auto& e : entities)
	{
		e->GetTransform().Rotate(XMFLOAT3(0.0f, 0.1f * deltaTime, 0.0f));
	}

	//Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(), color);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();

		//Draw all Entities
		for (size_t i = 0; i < entities.size(); i++) {
			//Pixel Shader
			//if the following values are not in the pixel shader,
			//SimpleShader ignores
			//pass in window height and width for entity's pixel shader
			float width = static_cast<float>(Window::Width());
			float height = static_cast<float>(Window::Height());
			entities[i]->GetMaterial()->GetPixelShader()->SetFloat2("resolution", XMFLOAT2(width, height));
			//pass in deltaTime for pixel shader
			entities[i]->GetMaterial()->GetPixelShader()->SetFloat("deltaTime", deltaTime);
			entities[i]->GetMaterial()->GetPixelShader()->SetFloat3("ambientColor", ambientLight);
			entities[i]->GetMaterial()->GetPixelShader()->SetInt("numLights", int(lights.size())); //number of lights
			entities[i]->GetMaterial()->GetPixelShader()->SetData("lights", //shader variable name
				&lights[0], //address of data
				sizeof(Lights) * //size of data structure
				(int)lights.size());
			//Drawing
			//draw entities
			entities[i]->Draw(cams[activeCam]);
		}

		floor->Draw(cams[activeCam]);

		defaultSky->Draw(cams[activeCam]);

		// UI is drawn last so it is on top
		DrawUI();

		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}

void Game::DrawUI()
{
	//Begin the window, everything until End() is included
	ImGui::Begin("Controls");

	//Give instructions
	ImGui::Text("Expand Dropdowns for Controls");

	//Container for framerate, window size, etc
	if (ImGui::CollapsingHeader("App Inspector"))
	{
		ImGui::Indent();

		if (ImGui::CollapsingHeader("App Data")) {
			ImGui::Indent();
			ImGui::SeparatorText("Window");
			// Replace the %f with the next parameter, and format as a float
			ImGui::Text("Framerate: %f fps", ImGui::GetIO().Framerate);
			// Replace each %d with the next parameter, and format as decimal integers
			// The "x" will be printed as-is between the numbers, like so: 800x600
			ImGui::Text("Window Resolution: %dx%d", Window::Width(), Window::Height());
			ImGui::Unindent();
		}

		if (ImGui::CollapsingHeader("Camera")) {
			ImGui::Text("Camera Data");
			XMFLOAT3 camPos = cams[activeCam]->GetTransform().GetPosition();

			// Display Camera Position
			ImGui::Text("Position: X=%.2f, Y=%.2f, Z=%.2f", camPos.x, camPos.y, camPos.z);

			float fov = XMConvertToDegrees(cams[activeCam]->GetFOV());

			// Display FOV
			ImGui::Text("Field of View: %.2f�", fov);
		}

		if (ImGui::CollapsingHeader("Mesh Data")) {
			ImGui::Indent();

			//Iterate over the vector of meshes
			for (size_t i = 0; i < entities.size(); i++) {
				std::shared_ptr<Mesh> mesh = entities[i]->GetMesh();

				//Generate a unique section title for each mesh
				std::string meshTitle = "Mesh " + std::to_string(i + 1);
				ImGui::SeparatorText(meshTitle.c_str());

				//Retrieve mesh data
				int vertexCount = mesh->GetVertexCount();
				int indexCount = mesh->GetIndexCount();
				int triangleCount = indexCount / 3;

				//Display mesh data
				ImGui::Text("Number of Vertices: %d", vertexCount);
				ImGui::Text("Number of Indices: %d", indexCount);
				ImGui::Text("Number of Triangles: %d", triangleCount);
			}


			ImGui::Unindent();
		}


		ImGui::Unindent();
	}

	//Continer for controls
	if (ImGui::CollapsingHeader("Controls"))
	{
		ImGui::SeparatorText("Toggle Cameras");
		for (size_t i = 0; i < cams.size(); i++)
		{
			std::string label = "Camera " + std::to_string(i + 1);

			// Create a radio button for each camera
			if (ImGui::RadioButton(label.c_str(), activeCam == i))
			{
				activeCam = (int)i; // Update active camera index
			}
		}

		//Entities
		if (ImGui::CollapsingHeader("Entities")) {
			// Alter Matrices of Entities
			for (size_t i = 0; i < entities.size(); i++)
			{
				// Generate a unique identifier for each entity control group
				std::string label = "Entity " + std::to_string(i + 1);
				if (ImGui::CollapsingHeader(label.c_str())) {

					ImGui::SeparatorText("Transform");

					// Retrieve transform reference
					Transform& transform = entities[i]->GetTransform();

					// Create variables for editing (ImGui needs modifiable data)
					XMFLOAT3 position = transform.GetPosition();
					XMFLOAT3 rotation = transform.GetRotation();
					XMFLOAT3 scale = transform.GetScale();

					// Modify Position
					if (ImGui::DragFloat3(("Position##" + std::to_string(i)).c_str(), &position.x, 0.1f, -100.0f, 100.0f))
					{
						transform.SetPosition(position);
					}

					// Modify Rotation (ImGui uses degrees, so convert from radians)
					XMFLOAT3 rotationDegrees = { XMConvertToDegrees(rotation.x),
												 XMConvertToDegrees(rotation.y),
												 XMConvertToDegrees(rotation.z) };

					if (ImGui::DragFloat3(("Rotation##" + std::to_string(i)).c_str(), &rotationDegrees.x, 1.0f, -360.0f, 360.0f))
					{
						transform.SetRotation(XMFLOAT3(XMConvertToRadians(rotationDegrees.x),
							XMConvertToRadians(rotationDegrees.y),
							XMConvertToRadians(rotationDegrees.z)));
					}

					// Modify Scale
					if (ImGui::DragFloat3(("Scale##" + std::to_string(i)).c_str(), &scale.x, 0.1f, 0.1f, 10.0f))
					{
						transform.SetScale(scale);
					}

					//Display Material Info
					ImGui::SeparatorText("Material");
					//store material
					std::shared_ptr<Material> mat = entities[i]->GetMaterial();
					//color
					XMFLOAT3 colorTint = mat->GetColor();
					if (ImGui::DragFloat3(("Color##" + std::to_string(i)).c_str(), &colorTint.x, 0.001f, 0.0f, 1.0f)) {
						entities[i]->GetMaterial()->SetColor(colorTint);
					}

					//Display texture info
					//uvoffset
					XMFLOAT2 uvOffset = mat->GetUvOffset();
					if (ImGui::DragFloat2(("UV Offset##" + std::to_string(i)).c_str(), &uvOffset.x, 0.01f, 0.1f, 10.0f)) {
						mat->SetUvOffset(uvOffset);
					}
					//scale
					XMFLOAT2 uvScale = mat->GetUvScale();
					if (ImGui::DragFloat2(("UV Scale##" + std::to_string(i)).c_str(), &uvScale.x, 0.01f, 0.1f, 10.0f)) {
						mat->SetUvScale(uvScale);
					}

					std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textures = entities[i]->GetMaterial()->GetTextureShaderResourceViewMap();
					for (auto& textureInfo : textures) {
						//get texture name
						std::string name = textureInfo.first;
						//get srv
						ID3D11ShaderResourceView* srv = textureInfo.second.Get();

						//check if the srv exists
						if (srv) {
							//display info about the texture
							ImGui::Text("%s", name.c_str());
							//display image of texture
							ImGui::Image((ImTextureID)srv, ImVec2(128, 128));
						}
					}
				}
			}
		}

		//Lights
		if (ImGui::CollapsingHeader("Lighting")) {
			ImGui::SeparatorText("Universal Lighting");
			ImGui::ColorPicker3("Ambient Light", &ambientLight.x);
			ImGui::DragFloat3("Background Color", &color[0], 0.001f, 0.0f, 1.5f);
			ImGui::SeparatorText("Light Sources");
			if (ImGui::CollapsingHeader("Lights")) {
				for (size_t i = 0; i < lights.size(); i++) {
					std::string label = "Light " + std::to_string(i + 1);
					if (ImGui::CollapsingHeader(label.c_str())) {
						//Could reorganize conditionals for efficency

						//all light types
						ImGui::ColorPicker3(("Color##" + std::to_string(i)).c_str(), &lights[i].color.x); //color
						ImGui::DragFloat(("Intensity##" + std::to_string(i)).c_str(), &lights[i].intensity, 0.01f, 0.0f, 1.5f); //intensity
						ImGui::DragFloat3(("Driection##" + std::to_string(i)).c_str(), &lights[i].direction.x, 0.1f, XMConvertToRadians(-360.0f), XMConvertToRadians(360.0f)); //direction

						//directional lights are done
						if (lights[i].type == LIGHT_TYPE_DIRECTIONAL) {
							continue;
						}

						//shared by point and spot
						ImGui::DragFloat3(("Position##" + std::to_string(i)).c_str(), &lights[i].position.x, 0.1f, -20.0f, 20.0f); //position
						ImGui::DragFloat(("Range##" + std::to_string(i)).c_str(), &lights[i].range, 0.1f, 0.0f, 50.0f); //range

						//point lights are done
						if (lights[i].type == LIGHT_TYPE_POINT) {
							continue;
						}

						//only spot lights
						ImGui::DragFloat(("Inner Spotlight##" + std::to_string(i)).c_str(), &lights[i].spotInnerAngle, 0.1f, XMConvertToRadians(-360.0f), XMConvertToRadians(360.0f)); //inner angle
						ImGui::DragFloat(("Outer Spotlight##" + std::to_string(i)).c_str(), &lights[i].spotOuterAngle, 0.1f, XMConvertToRadians(-360.0f), XMConvertToRadians(360.0f)); //inner angle
					}
				}
			}
		}
	}

	ImGui::End();

	//These lines go AFTER ui elements are created
	ImGui::Render(); // Turns this frame�s UI into renderable triangles
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen
}

void Game::ResetVertices() {
	////default colors
	//top = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	//left = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	//right = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	////default positions
	//topPosition = XMFLOAT3(0.0f, 0.5f, 0.0f);
	//leftPosition = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	//rightPosition = XMFLOAT3(0.5f, -0.5f, 0.0f);
}

