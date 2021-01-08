#version 430

in vec2 pos;
in vec2 posPx;

out vec4 FragColor;

void main() {
	FragColor = vec4(pos.x, 0, 0, 1.0);
}