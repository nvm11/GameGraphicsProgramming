#include <wrl/client.h> //comptr
#include <d3d11.h> //d3d11 objs
#include <memory> //smart pointers
#include "Mesh.h"
#include "SimpleShader.h" //shader info

#pragma once
class Sky
{
	//Fields
	//d3d11 objs
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler; //sampler options
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skySRV; //cube map texture
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthState; //depth buffer comparison type
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterState; //rasterizer options
	//shaders
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;
	//other objs
	std::shared_ptr<Mesh> cubeMesh;

	//Constructor
	//with already loaded in resources
	Sky(std::shared_ptr<Mesh> mesh,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler,
		std::shared_ptr<SimpleVertexShader> vs,
		std::shared_ptr<SimplePixelShader> ps,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skySRV);

	//loads in specific resources
	Sky(std::shared_ptr<Mesh> mesh,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler,
		const wchar_t* cubeMapPath);

	// --------------------------------------------------------
	// Author: Chris Cascioli
	// Purpose: Creates a cube map on the GPU from 6 individual textures
	// 
	// - You are allowed to directly copy/paste this into your code base
	//   for assignments, given that you clearly cite that this is not
	//   code of your own design.
	//
	// - Note: This code assumes you’re putting the function in Sky.cpp, 
	//   you’ve included WICTextureLoader.h and you have an ID3D11Device 
	//   ComPtr called “device”.  Make any adjustments necessary for
	//   your own implementation.
	// --------------------------------------------------------
	
	
	// --- HEADER ---
	
	// Helper for creating a cubemap from 6 individual textures
		static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);
};

