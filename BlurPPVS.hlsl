#include "Structs.hlsli"

VertexToPixelPP main(uint id : SV_VertexID)
{
    VertexToPixelPP output;
    //calculate the UV (0,0) to (2,2) using the ID
    output.uv = float2(
    (id << 1) & 2, // Essentially: id % 2 * 2
    id & 2);
    //calculate the position based on the UV
    output.position = float4(output.uv, 0, 1);
    output.position.x = output.position.x * 2 - 1;
    output.position.y = output.position.y * -2 + 1;

    return output;
}