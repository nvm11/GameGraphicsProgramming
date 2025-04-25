#include "Structs.hlsli"

VertexToPixelPP main(uint id : SV_VertexID)
{
    VertexToPixelPP output;
    if (id == 0)
    {
        output.position = float4(-1, 1, 0, 1);
        output.uv = float2(0, 0);
    }
    else if (id == 1)
    {
        output.position = float4(3, 1, 0, 1);
        output.uv = float2(2, 0);
    }
    else if (id == 2)
    {
        output.position = float4(-1, -3, 0, 1);
        output.uv = float2(0, 2);
    }
    return output;
}