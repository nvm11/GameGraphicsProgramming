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
	LoadShaders();
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

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
		Graphics::Context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		Graphics::Context->VSSetShader(vertexShader.Get(), 0, 0);
		Graphics::Context->PSSetShader(pixelShader.Get(), 0, 0);

		//Create a Constant buffer
		{
			//Create a constant buffer for color and offset
			//get size needed for memory allocation
			unsigned int size = sizeof(ShaderData);
			//ensure a large enough multiple of 16
			size = (size + 15) / 16 * 16;

			D3D11_BUFFER_DESC cbDesc = {}; //zero out the struct
			cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbDesc.ByteWidth = size;
			cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //tell api we need this to be in a writable location
			cbDesc.Usage = D3D11_USAGE_DYNAMIC; //contents change after creation

			//use description to create the buffer
			Graphics::Device->CreateBuffer(&cbDesc, 0, constantBuffer.GetAddressOf());


			//Bind the constant buffer
			Graphics::Context->VSSetConstantBuffers(
				0, //register
				1, //number of buffers
				constantBuffer.GetAddressOf()); //can be an array if there are multiple

			//set default for constant buffer
			vsData.colorTint = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		// Initialize ImGui itself & platform/renderer backends
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(Window::Handle());
		ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
		// Pick a style
		ImGui::StyleColorsDark();
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
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	// BLOBs (or Binary Large OBjects) for reading raw data from external files
	// - This is a simplified way of handling big chunks of external data
	// - Literally just a big array of bytes read from a file
	ID3DBlob* pixelShaderBlob;
	ID3DBlob* vertexShaderBlob;

	// Loading shaders
	//  - Visual Studio will compile our shaders at build time
	//  - They are saved as .cso (Compiled Shader Object) files
	//  - We need to load them when the application starts
	{
		// Read our compiled shader code files into blobs
		// - Essentially just "open the file and plop its contents here"
		// - Uses the custom FixPath() helper from Helpers.h to ensure relative paths
		// - Note the "L" before the string - this tells the compiler the string uses wide characters
		D3DReadFileToBlob(FixPath(L"PixelShader.cso").c_str(), &pixelShaderBlob);
		D3DReadFileToBlob(FixPath(L"VertexShader.cso").c_str(), &vertexShaderBlob);

		// Create the actual Direct3D shaders on the GPU
		Graphics::Device->CreatePixelShader(
			pixelShaderBlob->GetBufferPointer(),	// Pointer to blob's contents
			pixelShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			pixelShader.GetAddressOf());			// Address of the ID3D11PixelShader pointer

		Graphics::Device->CreateVertexShader(
			vertexShaderBlob->GetBufferPointer(),	// Get a pointer to the blob's contents
			vertexShaderBlob->GetBufferSize(),		// How big is that data?
			0,										// No classes in this shader
			vertexShader.GetAddressOf());			// The address of the ID3D11VertexShader pointer
	}

	// Create an input layout 
	//  - This describes the layout of data sent to a vertex shader
	//  - In other words, it describes how to interpret data (numbers) in a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the vertex shader blob above)
	{
		D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

		// Set up the first element - a position, which is 3 float values
		inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
		inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
		inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

		// Set up the second element - a color, which is 4 more float values
		inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
		inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
		inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

		// Create the input layout, verifying our description against actual shader code
		Graphics::Device->CreateInputLayout(
			inputElements,							// An array of descriptions
			2,										// How many elements in that array?
			vertexShaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
			vertexShaderBlob->GetBufferSize(),		// Size of the shader code that uses this layout
			inputLayout.GetAddressOf());			// Address of the resulting ID3D11InputLayout pointer
	}
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in CPU memory
	//    over to a Direct3D-controlled data structure on the GPU (the vertex buffer)
	// - Note: Since we don't have a camera or really any concept of
	//    a "3d world" yet, we're simply describing positions within the
	//    bounds of how the rasterizer sees our screen: [-1 to +1] on X and Y
	// - This means (0,0) is at the very center of the screen.
	// - These are known as "Normalized Device Coordinates" or "Homogeneous 
	//    Screen Coords", which are ways to describe a position without
	//    knowing the exact size (in pixels) of the image/window/etc.  
	// - Long story short: Resizing the window also resizes the triangle,
	//    since we're describing the triangle in terms of the window itself
	Vertex vertices[] =
	{
		{ XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, //Top vertex (red)
		{ XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }, //Right vertex (green)
		{ XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }  //Left vertex (blue)
	};

	// Set up indices, which tell us which vertices to use and in which order
	// - This is redundant for just 3 vertices, but will be more useful later
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int indices[] = { 0, 1, 2 };

	//Make a mesh using this data
	starterMesh = std::make_shared<Mesh>(vertices, indices, sizeof(vertices) / sizeof(Vertex), sizeof(indices) / sizeof(unsigned int));
	meshes.push_back(starterMesh);

	//Repeat the process two more times
	Vertex secondVertices[] =
	{
		{ XMFLOAT3(-0.9f, -0.1f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, // Top-left (red)
		{ XMFLOAT3(-0.7f, -0.1f, 0.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) }, // Top-right (cyan)
		{ XMFLOAT3(-0.7f, -0.9f, 0.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) }, // Bottom-right (magenta)
		{ XMFLOAT3(-0.9f, -0.9f, 0.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) }  // Bottom-left (yellow)
	};

	unsigned int secondIndices[] = {
		0, 1, 2,
		2, 3, 0
	};

	secondMesh = std::make_shared<Mesh>(secondVertices, secondIndices, sizeof(secondVertices) / sizeof(Vertex), sizeof(secondIndices) / sizeof(unsigned int));
	meshes.push_back(secondMesh);

	Vertex thirdVertices[] = {
		{ XMFLOAT3(0.0f,  0.5f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },  //Bottom left
		{ XMFLOAT3(0.25f, 1.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },  //Top left
		{ XMFLOAT3(0.5f,  0.5f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },  //Middle bottom
		{ XMFLOAT3(0.75f, 1.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },  //Top right
		{ XMFLOAT3(1.0f,  0.5f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }   //Bottom right
	};

	unsigned int thirdIndices[] = { 0,1,2,
									2,1,3,
									3,4,2 };

	thirdMesh = std::make_shared<Mesh>(thirdVertices, thirdIndices, sizeof(thirdVertices) / sizeof(Vertex), sizeof(thirdIndices) / sizeof(unsigned int));
	meshes.push_back(thirdMesh);
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
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

	// Example input checking: Quit if the escape key is pressed
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


	//handle the contant buffer mapping and unmapping
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	Graphics::Context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
	Graphics::Context->Unmap(constantBuffer.Get(), 0);


	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();

		//Draw the starter mesh
		starterMesh->Draw();
		//Draw second mesh
		secondMesh->Draw();
		//draw third mesh
		thirdMesh->Draw();

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

		if (ImGui::CollapsingHeader("Mesh Data")) {
			ImGui::Indent();

			//Iterate over the vector of meshes
			for (size_t i = 0; i < meshes.size(); i++) {
				std::shared_ptr<Mesh> mesh = meshes[i];

				//Generate a unique section title for each mesh
				std::string meshTitle = "Mesh " + std::to_string(i + 1);
				ImGui::SeparatorText(meshTitle.c_str());

				// Retrieve mesh data
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
		ImGui::SeparatorText("Defaults");
		//Toggle Demo Visibility
		ImGui::Checkbox("Show Demo UI", &showDemoUI);

		ImGui::SeparatorText("Color");
		//Color Selector
		ImGui::ColorEdit4("Background Color", color);

		//local variable to help convert from xmfloat to float and back
		float shaderColor[] = { vsData.colorTint.x,vsData.colorTint.y ,vsData.colorTint.z };
		ImGui::ColorEdit4("Color Tint", &vsData.colorTint.x);

		//moving meshes
		ImGui::SeparatorText("Movement");
		ImGui::SliderFloat3("Offset", &vsData.offset.x, -1.0f, 1.0f);


		//CreateGeometry() is called to update 
		//the buffer with new colors and positions


		//Sub-Header
		//ImGui::SeparatorText("Triangle Colors");
		//ImGui::Text("--Top Vertex--");
		//if (ImGui::SliderFloat("R - Top", &top.x, 0.0f, 1.0f)) CreateGeometry();
		//if (ImGui::SliderFloat("G - Top", &top.y, 0.0f, 1.0f)) CreateGeometry();
		//if (ImGui::SliderFloat("B - Top", &top.z, 0.0f, 1.0f)) CreateGeometry();
		//ImGui::Text("--Left Vertex--");
		//if (ImGui::SliderFloat("R - Left", &left.x, 0.0f, 1.0f)) CreateGeometry();
		//if (ImGui::SliderFloat("G - Left", &left.y, 0.0f, 1.0f)) CreateGeometry();
		//if (ImGui::SliderFloat("B - Left", &left.z, 0.0f, 1.0f)) CreateGeometry();
		//ImGui::Text("--Right Vertex--");
		//if (ImGui::SliderFloat("R - Right", &right.x, 0.0f, 1.0f)) CreateGeometry();
		//if (ImGui::SliderFloat("G - Right", &right.y, 0.0f, 1.0f)) CreateGeometry();
		//if (ImGui::SliderFloat("B - Right", &right.z, 0.0f, 1.0f)) CreateGeometry();

		////Positions
		//ImGui::SeparatorText("Position");
		//ImGui::Text("--Top Vertex--");
		//if (ImGui::SliderFloat("Top X", &topPosition.x, -1.0f, 1.0f)) CreateGeometry();
		//if (ImGui::SliderFloat("Top Y", &topPosition.y, -1.0f, 1.0f)) CreateGeometry();
		//ImGui::Text("--Left Vertex--");
		//if (ImGui::SliderFloat("Left X", &leftPosition.x, -1.0f, 1.0f)) CreateGeometry();
		//if (ImGui::SliderFloat("Left Y", &leftPosition.y, -1.0f, 1.0f)) CreateGeometry();
		//ImGui::Text("--Right Vertex--");
		//if (ImGui::SliderFloat("Right X", &rightPosition.x, -1.0f, 1.0f)) CreateGeometry();
		//if (ImGui::SliderFloat("Right Y", &rightPosition.y, -1.0f, 1.0f)) CreateGeometry();

		//Reset Position and Color
		//ImGui::SeparatorText("Reset");
		//if (ImGui::Button("Reset")) {
		//	//Reset positions and colors
		//	ResetVertices();
		//	//send to gpu
		//	CreateGeometry();
		//}
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

