#include "Structs.hlsli"
#include "Lighting.hlsli"

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
    
    //light objects
    Lights lights[MAX_LIGHTS];
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
    
    //normalize input normals
    input.normal = normalize(input.normal);
	
	//"sample" the texture and color
	//this gives output color
    float3 color = SurfaceTexture.Sample(BasicSampler, input.uv).rgb; //swizzle using logical indices
    //tint color with colorTint
    color *= colorTint;
    
    //Begin lighting calculations
    //include ambient lighting ONCE
    float3 totalLight = color * ambientColor;
    
    //angle the surface is viewed from
    float3 surfaceToCamera = normalize(cameraPos - input.worldPos);
    
    //loop through lights
    for (int i = 0; i < numLights; i++)
    {
        //grab a copy of the current index
        Lights currentLight = lights[i];
        //get direction from surface to light
        float3 surfaceToLight = -currentLight.direction;
        
        //Calculate all types of lighting
        float diffuse = DiffuseLight(input.normal, surfaceToLight);
        float specular = PhongSpecularLight(input.normal, surfaceToLight, surfaceToCamera, roughness);
        
        //normalize the direction to ensure consistent results
        currentLight.direction = normalize(currentLight.direction);
        //combine (add) the (normalized) direction to light, surface color, light color, and intensity 
        totalLight += (diffuse + specular) *
        currentLight.color * currentLight.intensity * color;
        
    }
	
	//return modified color
        return float4(totalLight, 1);
}