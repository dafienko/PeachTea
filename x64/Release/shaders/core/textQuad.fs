#version 430

in vec2 glyphSamplePos;
in vec2 posPx;

uniform int useClipBounds;
uniform ivec2 clipX;
uniform ivec2 clipY;

uniform float transparency;
uniform vec3 textColor;

layout (binding=0) uniform sampler2D spritesheet;

out vec4 FragColor;

void main() {
	bool shouldBeRendered = true;
	
	// test if fragment is outside of occlusion bounds
	if (useClipBounds == 1) {
		if (!(posPx.x > clipX.x && posPx.x < clipX.y && posPx.y > clipY.x && posPx.y < clipY.y)) {
			shouldBeRendered = false;
		}
	}
	
	if (shouldBeRendered) {
		vec3 texColor = vec3(texture(spritesheet, glyphSamplePos).rgb);
		
		float totalTextGamma = texColor.r + texColor.g + texColor.b;
		float totalColorGamma = textColor.r + textColor.g + textColor.b;
		
		if (totalTextGamma > 0) {
			FragColor = vec4(
				min(texColor.r, textColor.r),
				min(texColor.g, textColor.g),
				min(texColor.b, textColor.b),
				(1 - transparency) 
			);
		} else {
			FragColor = vec4(0, 0, 0, 0);
		}
	} else {

	}
}









