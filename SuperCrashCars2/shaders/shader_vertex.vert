#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

out vec3 fragColor;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

uniform mat4 TM;

void main() {
	fragColor = color;
	gl_Position = P * V * M * TM * vec4(pos, 1.0f);
}
