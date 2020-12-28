#version 430

in vec2 pos;
in vec2 posPx;
layout (binding=0) uniform sampler2D tex;

uniform ivec2 screenSize;
uniform int blurRadius;
uniform vec2 blurDir;
uniform float offsetScale;

out vec4 color;



const float c = .8f;

float Gaussian(float x) {
	return exp(-(x * x)/(.5 * c * c));
}

void main() {
	vec2 dir = vec2(blurDir.x / screenSize.x, blurDir.y / screenSize.y);
	
	float totalWeight = Gaussian(0);
	vec4 acc = texture(tex, pos) * totalWeight;
	
	for (int i = 1; i < blurRadius; i++) {
		float alpha = i;
		alpha /= blurRadius;
		float weight = Gaussian(alpha);
		
		acc += texture(tex, pos + dir * i * offsetScale) * weight;
		acc += texture(tex, pos + dir * -i * offsetScale) * weight;
		
		totalWeight += weight * 2; // * 2 because 2 pixels were added (one +dir and one -dir from center)
	}
	
	acc /= totalWeight;
	color = vec4(acc.xyz, 1);
}