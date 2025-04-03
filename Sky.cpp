#include "Sky.h"

Sky::Sky(std::shared_ptr<Mesh> mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler) 
	: cubeMesh(mesh), sampler(sampler)
{

}