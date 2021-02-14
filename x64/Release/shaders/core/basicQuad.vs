#version 430

layout (location=0) in vec2 screenPos;
layout (location=1) in vec2 quadCorner;

uniform ivec2 screenSize;
uniform float depth;

out vec2 pos;
out vec2 posPx;

void main() {
	pos = quadCorner;
	posPx = screenPos;
	
	float xAlpha = ((1.0f * screenPos.x) / screenSize.x) * 2.0f - 1.0f;
	float yAlpha = ((1.0f * screenPos.y) / screenSize.y) * -2.0f + 1.0f;
	gl_Position = vec4(xAlpha, yAlpha, 0, 1);
}