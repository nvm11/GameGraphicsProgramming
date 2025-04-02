#ifndef __SHADER_STRUCTS__
#define __SHADER_STRUCTS__

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float3 localPosition : POSITION; // XYZ position
    float2 uv : TEXCOORD; // UV texture coordinates
    float3 normal : NORMAL; // Surface normals for lighting
    float3 tangent : TANGENT; // Normal from normal map
};

// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION; // XYZ position
    float2 uv : TEXCOORD; // UV texture coordinates
    float3 normal : NORMAL; // Surface normals for lighting
    float3 worldPos : POSITION; //world space position
};

struct VertexToNormalMapPS
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION; // XYZ position
    float2 uv : TEXCOORD; // UV texture coordinates
    float3 normal : NORMAL; // Surface normals for lighting
    float3 worldPos : POSITION; //world space position
    float3 tangent : TANGENT; //tangent from normal map
};

#endif