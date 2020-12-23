#version 430

in vec2 pos;
in vec2 posPx;

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

out vec4 FragColor;

void main() {
	float xAlpha = min(pos.x, abs(1.0 - pos.x));
	float yAlpha = min(pos.y, abs(1.0 - pos.y));
	
	float dist = 0;
	if (reactive != 0) {
		dist = length(posPx - mousePos);
	}
	
	if (xAlpha < frameBorderComposition.x || yAlpha < frameBorderComposition.y) {
		vec3 color = borderColor;
		
		if (reactive != 0) {
			float activeRange = activeBorderRange.y - activeBorderRange.x;
			float alpha = 1 - clamp((dist - activeBorderRange.x) / activeRange, 0, 1);
			color = mix(color, activeBorderColor, alpha);
		}
		
		FragColor = vec4(color, 1 - borderTransparency);
	} else { // not on border
		vec3 color = backgroundColor;
		
		if (reactive != 0) {
			if (mouseInFrame > 0) {
				float activeRange = activeBackgroundRange.y - activeBackgroundRange.x;
				float alpha = 1 - clamp((dist - activeBackgroundRange.x) / activeRange, 0, 1);
				color = mix(color, activeBackgroundColor, alpha);
			}
		}
		
		FragColor = vec4(color, 1 - backgroundTransparency);
	}
}














