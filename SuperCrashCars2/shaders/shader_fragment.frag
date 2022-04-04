#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform sampler2D texture_diffuse1;
uniform sampler2D shadowMap;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;



float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // check whether current frag pos is in shadow
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightPos - FragPos);
    
   //float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
   
    float bias = 0.0001f;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  
   
    //float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);  
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;  

    return shadow;
}  



void main() 
{
    vec3 lightColor = vec3(1.0);
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;
    vec3 normal = normalize(Normal);
    
    // ambient
    vec3 ambient = 0.25 * lightColor;
    
    // diffuse
    vec3 lightDirection = normalize(lightPos - FragPos);
    float diff = max(dot(lightDirection, normal), 0.0f);
    vec3 diffuse = diff * lightColor;
    
    // specular
    vec3 viewDirection = normalize(camPos - FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDirection + viewDirection);  
    spec = pow(max(dot(normal, halfwayDir), 0.0f), 128.0f);
    vec3 specular = spec * lightColor;    
   
    // calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);

} 
