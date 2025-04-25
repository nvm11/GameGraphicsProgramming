#include "Structs.hlsli"

Texture2D Pixels : register(t0);
SamplerState ClampSampler : register(s0);
float4 main(VertexToPixelPP input) : SV_TARGET
{
    float4 pixelColor = Pixels.Sample(ClampSampler, input.uv);
// NOTE: Here is where you should actually "process" the image
    return pixelColor;
}