#include "Structs.hlsli"
#include "Lighting.hlsli"

//data to manipulate pixels
cbuffer ShaderData : register(b0)
{
	//material data
    //float3 colorTint;
    //float roughness;
    float2 uvScale; //changes how many times texture is on object
    float2 uvOffset; //changes start position of texture

	//entity data
    float3 cameraPos; //helps with specular + diffuse lighting

	//light data
    int numLights;

	//light objects
    Lights lights[MAX_LIGHTS];
};

Texture2D Albedo : register(t0); //whiteness map (surface texture)
Texture2D NormalMap : register(t1); //normal map
Texture2D RoughnessMap : register(t2); //roughness map (similar to specular maps)
Texture2D MetalnessMap : register(t3); //metalness map (0 or 1)

Texture2D ShadowMap : register(t4);

SamplerState BasicSampler : register(s0); //"s" registers for samplers
SamplerComparisonState ShadowSampler : register(s1); //comparison sampler

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToNormalMapPS input) : SV_TARGET
{
	
	//alter uv coords using offset
    input.uv = input.uv * uvScale + uvOffset;

	//normalize input normal
    input.normal = normalize(input.normal);
	//normalize input tangent
    input.tangent = normalize(input.tangent);
	
    input.normal = NormalFromMap(NormalMap, BasicSampler, input.uv, input.normal, input.tangent);
	

	//"sample" the texture and color
	//this gives output color
	//use pow for "unpacking" gamma correction
    float3 color = pow(Albedo.Sample(BasicSampler, input.uv).rgb, 2.2f); //swizzle using logical indices
	
	//detemrined by a single float, simply sample the red channel
    float roughnessFromMap = RoughnessMap.Sample(BasicSampler, input.uv).r;
	
	//also a single float
    float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;
	
	//now, a specular color is needed
	//metals generally tint reflections, while nonmetals generally have
	//the same specular color throughout (obtained using 0.04)
	//metalness is usually 0 or 1, bu might be inbetween because of
	//linear texture sampling, lerp is used on the specular color to match this
    float3 specularColor = lerp(F0_NON_METAL, color.rgb, metalness);
	
	//Shadow mapping
    float2 shadowUV = input.shadowMapPos.xy / input.shadowMapPos.w * 0.5f + 0.5f;
    shadowUV.y = 1.0f - shadowUV.y;
	
	//get pixel's depth from the shadow
    float depthFromLight = input.shadowMapPos.z / input.shadowMapPos.w;
	
	//sample shadow map with a COMPARISON sampler
	//compares depth value from the light and the value in shadow map
    float shadowAmount = ShadowMap.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight).r;
	
	//Begin lighting calculations
	//include ambient lighting ONCE
    float3 totalLight = float3(0, 0, 0);
	
	//angle the surface is viewed from
    float3 surfaceToCamera = normalize(cameraPos - input.worldPos);
	
	//apply the total lighting
    totalLight += CalculateTotalLightPBR(numLights, lights, input.normal, surfaceToCamera, input.worldPos, roughnessFromMap, metalness, color, specularColor, shadowAmount);
	
	//return modified color
    return float4(GammaCorrect(totalLight), 1);
}