#pragma once
#include "SimpleShader.h"
#include <memory>
#include <DirectXMath.h>
class Material
{
private:
	//fields
	DirectX::XMFLOAT4 colorTint;
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;

public:
	//constructor
	Material(std::shared_ptr<SimpleVertexShader> vs,
		std::shared_ptr<SimplePixelShader> ps,
		const DirectX::XMFLOAT4 initialColor);

	//getters
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	DirectX::XMFLOAT4 GetColor();

	//setters
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> newVs);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> newPs);
	void SetColor(const DirectX::XMFLOAT4& newColor);
};

