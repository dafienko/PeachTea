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
	
	//FragColor = vec4(1, 0, 0, 1);
	
	float dist = length(posPx - mousePos);
	
	
	if (xAlpha < frameBorderComposition.x || yAlpha < frameBorderComposition.y) {
		float alpha = clamp(pow(dist / 210.0, 2), 0, 1);
		vec3 mouseColor = vec3(1.0, 1.0, 1.0);
		
		vec3 c = mix(mouseColor, borderColor, alpha);
		
		FragColor = vec4(c, 1 - borderTransparency);
	} else { // not on border
		float alpha = 1;
		
		if (mouseInFrame) {
			alpha = clamp(pow(dist / 650.0, 1), 0, 1);
		}
		
		vec3 mouseColor = vec3(0, 35/510, .25);
		
		
		
		vec3 c = mix(mouseColor, color, alpha);
		
		FragColor = vec4(c, 1 - transparency);
	}
}














