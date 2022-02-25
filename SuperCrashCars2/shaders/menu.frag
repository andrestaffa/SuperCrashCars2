#version 330 core
out vec4 color;

in vec2 tc;

uniform sampler2D sampler;

void main() {
	vec4 d = texture(sampler, tc);
	if(d.a < 0.01)
        discard; // If the texture is transparent, don't draw the fragment
	color = d;
} 
