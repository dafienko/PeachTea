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
	int shouldBeRendered = 1;
	
	// test if fragment is outside of occlusion bounds
	if (useClipBounds == 1) {
		if (!(posPx.x > clipX.x && posPx.x < clipX.y && posPx.y > clipY.x && posPx.y < clipY.y)) {
			shouldBeRendered = 0;
		}
	}
	
	//FragColor = vec4(1, 0, 0, 1);
	if (shouldBeRendered == 1) {
		//float texColor = texture(spritesheet, glyphSamplePos).r;
		vec3 texColor = vec3(texture(spritesheet, glyphSamplePos).rgb);
		
		//FragColor = vec4(texture(spritesheet, glyphSamplePos).xyz, 1);
		float totalTextGamma = texColor.r + texColor.g + texColor.b;
		
		float totalColorGamma = textColor.r + textColor.g + textColor.b;
		float rComp = textColor.r / totalColorGamma;
		float gComp = textColor.g / totalColorGamma;
		float bComp = textColor.b / totalColorGamma;
		
		if (totalTextGamma > 0) {
			if (totalTextGamma == 3) {
				FragColor = vec4(textColor.rgb, 1 - transparency);
			} else {
				FragColor = vec4(
					texColor.r * rComp * 3,
					texColor.g * gComp * 3,
					texColor.b * bComp * 3,
					(1 - transparency) //* (totalTextGamma * .333)
				);
			}
		} else {
			FragColor = vec4(0, 0, 0, 0);
		}
		//if (texColor == 0) {
		//	FragColor = vec4(1, 0, 0, 1);
		//}
		//FragColor = vec4(0, 0, 1, 1);
	} else {
		//FragColor = vec4(0, 1, 0, 1);
	}
}









