#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 crntPos;

uniform sampler2D texture_diffuse1;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

void main() 
{
	// Ambient
	float ambient = 0.20f;

	// Diffuse
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// Specular
	float specular = 0.0f;
	if (diffuse != 0.0f)
	{
		float specularLight = 0.50f;
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 reflectionDirection = reflect(-lightDirection, normal);
		vec3 halfwayVec = normalize(viewDirection + lightDirection);
		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 8f);
		float specular = specAmount * specularLight;
	}
	

	FragColor = texture(texture_diffuse1, TexCoords) * lightColor * (diffuse + ambient + specular);
} 
