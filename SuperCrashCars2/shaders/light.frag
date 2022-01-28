#version 330 core

out vec4 FragColor;

uniform vec4 lightColor;

in vec2 TexCoords;

void main()
{
	FragColor = lightColor;
}