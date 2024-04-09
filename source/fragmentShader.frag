#version 330 core

#define maxLights 10

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 fragmentPosition;
in vec3 lightPositions[maxLights];
in vec3 lightDirections[maxLights];
in vec3 Normal;

// Output data
out vec3 fragmentColour;

// Structs
struct Light
{
    vec3 position;
    vec3 direction;
    vec3 colour;
    float linear, quadratic, constant;
    float cosPhi;
};

// Uniforms
uniform sampler2D diffuseMap;
uniform Light lights[maxLights];
uniform int numPoint;
uniform int numSpot;
uniform int numDir;
uniform float ka;
uniform float kd;
uniform float ks;
uniform float Ns;

// Function prototypes
vec3 calculatePointLight(vec3 fragmentPosition, vec3 lightPosition, vec3 normal, vec3 eye, Light light);
vec3 calculateSpotlight(vec3 fragmentPosition, vec3 lightPosition, vec3 lightDirection, vec3 normal, vec3 eye, Light light);
vec3 calculateDirectionalLight(vec3 lightDirection, vec3 normal, vec3 eye, Light light);

// Calculate normal and eye vectors (these are the same for all light sources)
vec3 normal = normalize(Normal);
vec3 eye = normalize(-fragmentPosition);

void main ()
{
    // Loop through the point light sources
    fragmentColour = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < numPoint; i++)
        fragmentColour += calculatePointLight(fragmentPosition, lightPositions[i], normal, eye, lights[i]);
    
    // Loop through the spotlight sources
    for (int i = numPoint; i < numPoint + numSpot; i++)
        fragmentColour += calculateSpotlight(fragmentPosition, lightPositions[i], lightDirections[i], normal, eye, lights[i]);
    
    // Loop through the directional light sources
    for (int i = numPoint + numSpot; i < numPoint + numSpot + numDir; i++)
        fragmentColour += calculateDirectionalLight(lightDirections[i], normal, eye, lights[i]);
}

// Calculate point light
vec3 calculatePointLight(vec3 fragmentPosition, vec3 lightPosition, vec3 normal, vec3 eye, Light light)
{
    // Object colour
    vec3 objectColour = texture(diffuseMap, UV).rgb;
    
    // Ambient reflection
    vec3 ambient = ka * light.colour * objectColour;
    
    // Diffuse reflection
    vec3 lightVector = normalize(lightPosition - fragmentPosition);
    float cosTheta = max(dot(lightVector, normal), 0.0);
    vec3 diffuse = kd * light.colour * objectColour * cosTheta;
    
    // Specular reflection
    vec3 reflection = -lightVector + 2.0 * dot(lightVector, normal) * normal;
    float cosAlpha = max(dot(eye, reflection), 0.0);
    vec3 specular = ks * light.colour * pow(cosAlpha, Ns);
    
    // Attenuation
    float distance = length(lightPosition - fragmentPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
 
    // Return fragment colour
    return (ambient + diffuse + specular) * attenuation;
}

// Calculate spotlight
vec3 calculateSpotlight(vec3 fragmentPosition, vec3 lightPosition, vec3 lightDirection, vec3 normal, vec3 eye, Light light)
{
    // Object colour
    vec3 objectColour = texture(diffuseMap, UV).rgb;
    
    // Ambient reflection
    vec3 ambient = ka * light.colour * objectColour;
    
    // Diffuse reflection
    vec3 lightVector = normalize(lightPosition - fragmentPosition);
    float cosTheta = max(dot(lightVector, normal), 0.0);
    vec3 diffuse = kd * light.colour * objectColour * cosTheta;
    
    // Specular reflection
    vec3 reflection = -lightVector + 2.0 * dot(lightVector, normal) * normal;
    float cosAlpha = max(dot(eye, reflection), 0.0);
    vec3 specular = ks * light.colour * pow(cosAlpha, Ns);
    
    // Attenuation
    float distance = length(lightPosition - fragmentPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    
    // Spotlight intensity
    vec3 direction = normalize(lightDirection);
    cosTheta = dot(lightVector, -direction);
    float delta = radians(2.0);
    float intensity = clamp((cosTheta - light.cosPhi) / delta, 0.0, 1.0);
    
    // Return fragment colour
    return ambient * attenuation + (diffuse + specular) * attenuation * intensity;
}

// Calculate directional light
vec3 calculateDirectionalLight(vec3 lightDirection, vec3 normal, vec3 eye, Light light)
{
    // Object colour
    vec3 objectColour = texture(diffuseMap, UV).rgb;
    
    // Ambient reflection
    vec3 ambient = ka * light.colour * objectColour;
    
    // Diffuse reflection
    vec3 lightVector = normalize(-lightDirection);
    float cosTheta = max(dot(lightVector, normal), 0.0);
    vec3 diffuse = kd * light.colour * objectColour * cosTheta;
    
    // Specular reflection
    vec3 reflection = -lightVector + 2.0 * dot(lightVector, normal) * normal;
    float cosAlpha = max(dot(eye, reflection), 0.0);
    vec3 specular = ks * light.colour * pow(cosAlpha, Ns);
 
    // Return fragment colour
    return ambient + diffuse + specular;
}
