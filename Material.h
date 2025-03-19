#pragma once
#include "SimpleShader.h"
#include <memory>
#include <DirectXMath.h>
//Maps for textures
#include <unordered_map>
class Material
{
private:
	//fields
	DirectX::XMFLOAT3 colorTint;
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs; //for textures (optional)
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;

public:
	//constructor
	Material(std::shared_ptr<SimpleVertexShader> vs,
		std::shared_ptr<SimplePixelShader> ps,
		const DirectX::XMFLOAT3 initialColor);

	//getters
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	DirectX::XMFLOAT3 GetColor();
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> GetTextureShaderResourceViewMap();
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> GetSamplerMap();

	//setters
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> newVs);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> newPs);
	void SetColor(const DirectX::XMFLOAT3& newColor);

	//adding textures
	void AddTextureSRV(std::string name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void AddSampler(std::string name, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);

	//preparing material for drawing
	void PrepareMaterial();
};

