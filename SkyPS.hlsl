#include "Structs.hlsli"

TextureCube SkyBox : register(t0);
SamplerState BasicSampler : register(s0);

float4 main(VertexToPixelSky input) : SV_TARGET
{
    //simply return a sample of the skybox
    return SkyBox.Sample(BasicSampler, input.sampleDir);
}