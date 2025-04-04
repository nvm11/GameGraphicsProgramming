#include "Structs.hlsli"

cbuffer ShaderData : register(b0)
{
    matrix view;
    matrix projection;
};

VertexToPixelSky main(VertexShaderInput input)
{
    //create struct to pass through
    VertexToPixelSky output;
    
    //copy the view matrix
    matrix viewCopy = view;
    
    //zero out translations
    viewCopy._14 = 0;
    viewCopy._24 = 0;
    viewCopy._34 = 0;
    
    //combine matrices
    matrix viewProj = mul(projection, viewCopy);
    //apply to local position
    output.position = mul(viewProj, float4(input.localPosition, 1.0f));
    
    //ensure z position of vertex is 1.0f after divide
    output.position.z = output.position.w;
    
    //set position as the direction 
    //to sample from the skybox
    output.sampleDir = input.localPosition;
    
    return output; //pass data along
}