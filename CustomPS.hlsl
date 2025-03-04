
//data to manipulate pixels
cbuffer ShaderData : register(b0)
{
    float3 colorTint;
    float deltaTime; //time of frame
    float2 resolution; //size of window
};

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
	//inspired by a shader made by The_King_Of_Shader
	//https://www.shadertoy.com/view/WcXSR2
	
    // Convert fragCoord from GLSL to HLSL equivalent
    float2 fragCoord = input.screenPosition.xy;
    
    // Normalize UV coordinates (centered around 0,0)
    float2 uv = (fragCoord - float2(0.5, 0.5) * resolution.xy) / resolution.xx;
    float2 uv2 = (fragCoord - float2(0.5, 0.505) * resolution.xy) / resolution.xx;
    float2 uv3 = (fragCoord - float2(0.505, 0.5) * resolution.xy) / resolution.xx;

    // Calculate animated circular wave pattern
    float Circle = (length(uv) + (5.0 * (deltaTime * -0.02))) * 40.0;
    float Circle2 = (length(uv2) + (5.0 * (deltaTime * -0.02))) * 40.0;
    float Circle3 = (length(uv3) + (5.0 * (deltaTime * -0.02))) * 40.0;

    // Sinusoidal variation
    float Cc = (sin(Circle) + 1.0) / 2.0;
    float Cc2 = (sin(Circle2) + 1.0) / 2.0;
    float Cc3 = (sin(Circle3) + 1.0) / 2.0;

    // Compute intensity variation
    float intensity = 10.0;
    float Cy = (Cc - Cc2) * intensity;
    float Cx = (Cc - Cc3) * intensity;

    // Compute normal-like effect
    float3 Result = normalize(cross(float3(1, 0, Cy), float3(0, 1, Cx)));

    // Apply color tint (red as in GLSL)
    Result *= float3(1.0, 0.0, Result.z * 10.0);

    return float4(Result, 1.0);

}