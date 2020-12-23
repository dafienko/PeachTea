#version 430

in vec2 pos;
in vec2 posPx;

uniform vec3 imageTint;
uniform float imageTintAlpha;
uniform float imageTransparency;

uniform ivec2 mousePos;
uniform int mouseInFrame;

uniform int reactive;
uniform vec3 activeBackgroundColor;
uniform vec2 activeBackgroundRange;

layout (binding=0) uniform sampler2D tex;

out vec4 FragColor;

void main() {
	vec4 imageColor = texture(tex, pos);
	vec3 c = mix(imageColor.xyz, imageTint, imageTintAlpha).xyz;
	
	float dist = 0;
	if (reactive != 0) {
		dist = length(posPx - mousePos);
	}
	
	if (reactive != 0) {
		if (mouseInFrame > 0) {
			float activeRange = activeBackgroundRange.y - activeBackgroundRange.x;
			float alpha = 1 - clamp((dist - activeBackgroundRange.x) / activeRange, 0, 1);
			c = mix(c, activeBackgroundColor, alpha);
		}
		
		
	}
	
	FragColor = vec4(c.xyz, imageColor.a * (1 - imageTransparency));
}