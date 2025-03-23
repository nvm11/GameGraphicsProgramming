#include "Structs.hlsli"

//data to manipulate pixels
cbuffer ShaderData : register(b0)
{
	//material data
    float3 colorTint;
    float roughness;
    float2 uvScale; //changes how many times texture is on object
    float2 uvOffset; //changes start position of texture
    
    //entity data
    float3 cameraPos; //helps with specular + diffuse lighting
    
    //light data
    int numLights;
    float3 ambientColor;
};

Texture2D SurfaceTexture : register(t0); // "t" registers for textures
SamplerState BasicSampler : register(s0); // "s" registers for samplers

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	//alter uv coords using offset
    input.uv = input.uv * uvScale + uvOffset;
	
	//"sample" the texture and color
	//this gives output color
    float3 color = SurfaceTexture.Sample(BasicSampler, input.uv).rgb; //swizzle using logical indices
    color *= colorTint * ambientColor;
	
	//return modified color
    return float4(color, 1);
}