#version 430

in vec2 pos;
in vec2 posPx;

uniform vec3 color;
uniform float transparency;

out vec4 FragColor;

void main() {
	FragColor = vec4(color.xyz, 1 - transparency);
}