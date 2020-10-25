#version 430

in vec2 pos;
in vec2 posPx;

// x = percent of frame taken up by left border on x-axis
// y = percent of frame taken up by top border on y-axis
uniform vec2 frameBorderComposition; 
uniform vec3 borderColor;
uniform float borderTransparency;
uniform vec3 color;
uniform float transparency;
uniform ivec2 mousePos;
uniform int mouseInFrame;

out vec4 FragColor;

void main() {
	float xAlpha = min(pos.x, abs(1.0 - pos.x));
	float yAlpha = min(pos.y, abs(1.0 - pos.y));
	
	float dist = length(posPx - mousePos);
	
	
	if (xAlpha < frameBorderComposition.x || yAlpha < frameBorderComposition.y) {
		float alpha = 1 - clamp(pow(dist / 50.0, 3), 0, 1);
		vec3 c = mix(color, borderColor, alpha);
		
		if (alpha > .1) {
			FragColor = vec4(c, 1 - borderTransparency);
		} else {
			FragColor = vec4(0, 0, 0, 0);
		}
	} else { // not on border
		float alpha = 0;
		
		if (mouseInFrame > 0) {
			alpha = max((1 - clamp(pow(dist / 75.0, 2), 0, 1)), .4);
		}
		
		vec3 mouseColor = vec3(1, 1, 1);
		
		vec3 c = mix(color, mouseColor, alpha);
		
		FragColor = vec4(c, 1 - transparency);
	}
}














