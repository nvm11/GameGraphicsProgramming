#include "Material.h"

Material::Material(std::shared_ptr<SimpleVertexShader> vs, std::shared_ptr<SimplePixelShader> ps, const DirectX::XMFLOAT3 initialColor)
	:vs(vs), ps(ps), colorTint(initialColor)
{
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
	return vs;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
	return ps;
}

DirectX::XMFLOAT3 Material::GetColor()
{
	return colorTint;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> newVs)
{
	vs = newVs;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> newPs)
{
	ps = newPs;
}

void Material::SetColor(const DirectX::XMFLOAT3& newColor)
{
	colorTint = newColor;
}
