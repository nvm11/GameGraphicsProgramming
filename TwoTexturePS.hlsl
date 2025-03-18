
//data to manipulate pixels
cbuffer ShaderData : register(b0)
{
	//material data
    float3 colorTint;
    float2 uvScale; //changes how many times texture is on object
    float2 uvOffset; //changes start position of texture
};

Texture2D SurfaceTexture : register(t0); // "t" registers for textures
Texture2D TopTexture : register(t1); // texture "on top"
SamplerState BasicSampler : register(s0); // "s" registers for samplers

// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

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
	//create a texture that combines the surface and top
    float4 surface = SurfaceTexture.Sample(BasicSampler, input.uv);
    float4 top = TopTexture.Sample(BasicSampler, input.uv);
	//this gives output color
    float3 color = top.rgb * surface.rgb; //swizzle using logical indices
    color *= colorTint;
	
	//return modified color
    return float4(color, 1);
}