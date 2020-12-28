#version 430

layout (binding=0) uniform sampler2D blurTex;

in vec2 pos;
in vec2 posPx;
uniform ivec2 screenSize;

uniform vec3 backgroundColor;
uniform float backgroundTransparency;

// x = percent of frame taken up by left border on x-axis
// y = percent of frame taken up by top border on y-axis
uniform vec2 frameBorderComposition; 
uniform vec3 borderColor;
uniform float borderTransparency;

uniform ivec2 mousePos;
uniform int mouseInFrame;

uniform int reactive;
uniform vec3 activeBorderColor;
uniform vec3 activeBackgroundColor;
uniform vec2 activeBorderRange;
uniform vec2 activeBackgroundRange;

uniform int blurred;
uniform int blurRadius;
uniform float blurAlpha;

uniform int useClipBounds;
uniform ivec2 clipX;
uniform ivec2 clipY;

out vec4 FragColor;

void main() {
	int shouldBeRendered = 1;
	
	if (useClipBounds == 1) {
		if (!(posPx.x > clipX.x && posPx.x < clipX.y && posPx.y > clipY.x && posPx.y < clipY.y)) {
			shouldBeRendered = 0;
		}
	}
	
	if (shouldBeRendered == 1) {
		float transparency = 1.0f;
		vec3 color;
		
		float xAlpha = min(pos.x, abs(1.0 - pos.x));
		float yAlpha = min(pos.y, abs(1.0 - pos.y));
		
		float dist = 0;
		if (reactive != 0) {
			dist = length(posPx - mousePos);
		}
		
		// calculate reactive effect
		if (xAlpha < frameBorderComposition.x || yAlpha < frameBorderComposition.y) {
			color = borderColor;
			
			if (reactive != 0) {
				float activeRange = activeBorderRange.y - activeBorderRange.x;
				float alpha = 1 - clamp((dist - activeBorderRange.x) / activeRange, 0, 1); 
				color = mix(color, activeBorderColor, alpha);
			}
			
			transparency = 1 - borderTransparency;
		} else { // not on border
			color = backgroundColor;
			
			if (reactive != 0) {
				if (mouseInFrame > 0) {
					float activeRange = activeBackgroundRange.y - activeBackgroundRange.x;
					float alpha = 1 - clamp((dist - activeBackgroundRange.x) / activeRange, 0, 1);
					color = mix(color, activeBackgroundColor, alpha);
				}
			}
			
			transparency = 1 - backgroundTransparency;
		}
		
		
		//calculate blur effect
		if (blurred > 0) {
			vec2 screenPosAlpha = posPx / screenSize;
			vec3 blurColor = texture(blurTex, screenPosAlpha).xyz;
			color = mix(color, blurColor, blurAlpha);
		}
		
		FragColor = vec4(color, transparency);
	}
}














