#version 430

in vec2 pos;
in vec2 posPx;

layout (binding=0) uniform sampler2D tex;

//out float FragColor;
out vec4 FragColor;

void main() {
	float tex = texture(tex, pos).r;

	FragColor = vec4(tex, 0, 0, 1);
}