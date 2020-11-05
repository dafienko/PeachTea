#version 430

in vec2 pos;
in vec2 posPx;

uniform float transparency;
uniform vec3 color;

layout (binding=0) uniform sampler2D tex;

out vec4 FragColor;

void main() {
	FragColor = vec4(color, texture(tex, pos).r * (1 - transparency));

}