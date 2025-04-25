#include "Structs.hlsli"

cbuffer externalData : register(b0)
{
    int blurRadius;
    float pixelWidth;
    float pixelHeight;
}

Texture2D Pixels : register(t0);
SamplerState ClampSampler : register(s0);
float4 main(VertexToPixelPP input) : SV_TARGET
{
    // Track the total color and number of samples
    float4 total = 0;
    int sampleCount = 0;
// Loop through the "box"
    for (int x = -blurRadius; x <= blurRadius; x++)
    {
        for (int y = -blurRadius; y <= blurRadius; y++)
        {
            //calculate the uv for this sample
            float2 uv = input.uv;
            uv += float2(x * pixelWidth, y * pixelHeight);
            //add this color to the running total
            total += Pixels.Sample(ClampSampler, uv);
            sampleCount++;
        }
    }
    //return the average
    return total / sampleCount;
}