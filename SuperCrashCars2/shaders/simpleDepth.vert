#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 TM;

void main()
{
    gl_Position = lightSpaceMatrix * TM * vec4(aPos, 1.0);
}  