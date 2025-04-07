#ifndef __LIGHTING__
#define __LIGHTING__

//File contains all functions necessary to light a scene

//Types of Lights
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT	   1
#define LIGHT_TYPE_SPOT		   2

//Max number of lights in a scene (power of 2)
#define MAX_LIGHTS 64

#define MAX_SPECULAR_EXPONENT 256.0f

//see Lights.h for explanation of fields
struct Lights
{
    //64 bytes (power of 2 is faster)
    int type;
    float3 direction;
    float range;
    float3 position;
    float intensity;
    float3 color;
    float spotInnerAngle;
    float spotOuterAngle;
    float2 padding;
};

    //calculate diffuse lighting
float DiffuseLight(float3 normal, float3 directionToLight)
{
    //automatically normalizes values
    return saturate(dot(normal, directionToLight)); //return direction light "bounces" off object
}

float PhongSpecularLight(float3 normal, float3 directionToLight, float3 surfaceToCamera, float roughness)
{
    //Calculate necessary vectors
    //reflect the light across the normal (direction light leaves surface)
    float3 r = reflect(-directionToLight, normal);
    
    //Create an exponent value to represent the roughness of the surface
    //high value = shiny, low = dull
    float specularExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    
    //Perform specular calculation
    return pow(max(dot(r, surfaceToCamera), 0.0f), specularExponent);
}
//softens the light
float Attenuate(Lights currentLight, float3 worldPos)
{
    float dist = distance(currentLight.position, worldPos);
    float attenuation = saturate(1.0f - (dist * dist / (currentLight.range * currentLight.range)));
    return attenuation * attenuation;
}

//performs all necessary calculations for a directional light
float3 DirectionLight(Lights currentLight, float3 normal, float3 surfaceToCamera, float roughness, float3 color) //can pass in a scalar to effect specular light on a per-pixel basis
{
    //get direction from surface to light
    float3 surfaceToLight = -currentLight.direction;
    
    //Perform all lighting calculations (ambient is in main)
    float diffuse = DiffuseLight(normal, surfaceToLight);
    float specular = PhongSpecularLight(normal, surfaceToLight, surfaceToCamera, roughness);
    
    //return calculated light
    return (diffuse * color + specular) * currentLight.intensity * currentLight.color;
}
//performs all necessary calculations for a point light
float3 PointLight(Lights currentLight, float3 normal, float3 surfaceToCamera, float3 worldPosition, float roughness, float3 color)
{
    //get distance from light
    float3 surfaceToLight = normalize(currentLight.position - worldPosition);
    
    //Preform all lighting calculations
    float diffuse = DiffuseLight(normal, surfaceToLight);
    float specular = PhongSpecularLight(normal, surfaceToLight, surfaceToCamera, roughness);
    float attenuation = Attenuate(currentLight, worldPosition);
    
    //return lighting results
    return (diffuse * color + specular) * attenuation * currentLight.intensity;

}
//performs all necessary calculations for a spotlight
float3 SpotLight(Lights currentLight, float3 normal, float3 surfaceToCamera, float3 worldPosition, float roughness, float3 color)
{
    //get necessary components
    float3 surfaceToLight = normalize(currentLight.position - worldPosition);
    float3 angle = saturate(dot(-surfaceToLight, currentLight.direction));
    
    //get two "cones"
    float cosInner = cos(currentLight.spotInnerAngle);
    float cosOuter = cos(currentLight.spotOuterAngle);
    float falloffRange = cosOuter - cosInner;
    
    //simulate shrinking of the lights to form cone
    float spotTerm = saturate((cosOuter - angle) / falloffRange).x;
    
    //use terms with point light calculation to create the cone
    return PointLight(currentLight, normal, surfaceToCamera, worldPosition, roughness, color) * spotTerm;
}

float3 CalculateTotalLight(int numLights, Lights lights[MAX_LIGHTS], float3 normal, float3 surfaceToCamera, float3 worldPos, float roughness, float3 color)
{
    
    float3 totalLight; //store total lighting
    //loop through lights
    for (int i = 0; i < numLights; i++)
    {
        //grab a copy of the current index
        Lights currentLight = lights[i];
        //normalize the direction to ensure consistent results
        currentLight.direction = normalize(currentLight.direction);
        
        switch (currentLight.type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += DirectionLight(currentLight, normal, surfaceToCamera, roughness, color);
                break;
            case LIGHT_TYPE_POINT:
                totalLight += PointLight(currentLight, normal, surfaceToCamera, worldPos, roughness, color);
                break;
            case LIGHT_TYPE_SPOT:
                totalLight += SpotLight(currentLight, normal, surfaceToCamera, worldPos, roughness, color);
                break;
        }
    }
    
    return totalLight;
}

float3 NormalFromMap(Texture2D normalMap, SamplerState sample, float2 uv, float3 normal, float3 tangent)
{
    //sample the normal map and "unpack" result
    float3 normalMapData = normalMap.Sample(sample, uv).rgb * 2.0f - 1.0f;
    
    //build the tbn matrix
	//t = normalized tangent along u
	//b = bitangent (cross of t and n) along v
	//n = normalized surface normal
    float3 t = normalize(tangent - normal * dot(tangent, normal));
    float3x3 tbn = float3x3(t, cross(t, normal), normal);
    
    //put normal map value in world space using tbn
    return normalize(mul(normalMapData, tbn));
}

float FresnelReflection(float3 view, float3 surfaceNormal, float specularValue)
{
    //uses Schilick's aproxximation of the Fresnel term:
    //F(n, v, f0) = f0 + (1 - f0)(1 - (n dot v))^5
    //N == normal vector
    //V = view vector
    //f0 = Specular value (usually 0.0f for non metals)
    
    //dot product the normal with the view
    float NdotV = saturate(dot(surfaceNormal, view)); //makes light different at glancing angles
    return specularValue + (1 - specularValue) * pow(1 - NdotV, 5); //return reflection strength
}

float3 ApplyFresnelReflection(float3 cameraPos, float3 worldPos, float3 surfaceNormal, float3 totalLight, SamplerState BasicSampler, TextureCube Skybox, float specularValue)
{
    //get view vector
    float3 view = normalize(cameraPos - worldPos);
    //get reflection of light (camera to pixel vector)
    float3 reflection = reflect(-view, surfaceNormal);
    //combine with skybox color, surface color, and reflection strength
    return lerp(totalLight, Skybox.Sample(BasicSampler, reflection).rgb, FresnelReflection(view, surfaceNormal, specularValue));
}

#endif