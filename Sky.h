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
	Sky(std::shared_ptr<Mesh> mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
};

