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

float3 PhongSpecularLight(float3 normal, float3 directionToLight, float3 surfaceToCamera, float roughness)
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

#endif