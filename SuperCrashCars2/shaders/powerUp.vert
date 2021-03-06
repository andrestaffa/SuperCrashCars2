#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;

uniform mat4 V;
uniform mat4 P;
uniform mat4 TM; // model
uniform mat4 lightSpaceMatrix;

void main() 
{
	FragPos = vec3(TM * vec4(aPos, 1.0f));

	TexCoords = aTexCoords; 
	gl_Position = P * V * TM * vec4(aPos, 1.0f);
	Normal = aNormal;

	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);

}
