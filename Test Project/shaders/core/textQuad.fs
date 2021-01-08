#version 430

in vec2 pos;
in vec2 posPx;

uniform ivec2 charSize;

uniform int useClipBounds;
uniform ivec2 clipX;
uniform ivec2 clipY;

uniform float transparency;
uniform vec3 color;

layout (binding=0) uniform sampler2D tex;

out vec4 FragColor;

void main() {
	int shouldBeRendered = 1;
	
	if (useClipBounds == 1) {
		if (!(posPx.x > clipX.x && posPx.x < clipX.y && posPx.y > clipY.x && posPx.y < clipY.y)) {
			shouldBeRendered = 0;
		}
	}
	
	if (shouldBeRendered == 1) {
		float texColor = texture(tex, pos).r;
		
		ivec2 samplePos = ivec2(charSize.x * pos.x, charSize.y * pos.y);
		texColor = texelFetch(tex, samplePos, 0).r;
		
		FragColor = vec4(color, texColor * (1 - transparency));
	} else {
		FragColor = vec4(0, 0, 0, 0);
	}
}