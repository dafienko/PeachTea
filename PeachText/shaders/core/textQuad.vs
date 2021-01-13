#version 430

layout (location=0) in vec2 screenPos;
layout (location=1) in vec2 texPos; // (0, 0) | (1, 0) | (0, 1) | (1, 1)

uniform ivec2 screenSize;

out vec2 posPx;
out vec2 glyphSamplePos;

void main() {
	glyphSamplePos = texPos;

	posPx = screenPos;
	
	float xAlpha = ((1.0f * screenPos.x) / screenSize.x) * 2.0f - 1.0f;
	float yAlpha = ((1.0f * screenPos.y) / screenSize.y) * -2.0f + 1.0f;
	gl_Position = vec4(xAlpha, yAlpha, 0, 1);
}