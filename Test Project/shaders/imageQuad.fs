#version 430

in vec2 pos;
in vec2 posPx;

uniform float transparency;
uniform vec3 color;

layout (binding=0) uniform sampler2D tex;

out vec4 FragColor;

void main() {
	vec4 c = texture(tex, pos);
	
	FragColor = vec4(c.r * color.r, c.g * color.g, c.b * color.b, c.a * (1 - transparency));
}