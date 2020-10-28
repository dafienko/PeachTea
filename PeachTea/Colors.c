#include "Colors.h"
#include <math.h>

// "constructors"
PT_COLOR PT_COLOR_fromRGB(int r, int g, int b) {
	PT_COLOR c = { 0 };

	c.r = r / 255.0f;
	c.g = g / 255.0f;
	c.b = b / 255.0f;

	return c;
}

// source: https://www.rapidtables.com/convert/color/hsv-to-rgb.html
PT_COLOR PT_COLOR_fromHSV(float h, float s, float v) { //  0 <= h <= 360;   0 <= s & h <= 1;
	float C = v * s;
	float x = C * (1 - abs(fmod(h / 60.0f, 2.0f) - 1));
	float m = v - C;

	if (h < 60.0f) {
		return PT_COLOR_new(C, x, 0);
	} 
	else if (h < 120) {
		return PT_COLOR_new(x, C, 0);
	}
	else if (h < 180) {
		return PT_COLOR_new(0, C, x);
	}
	else if (h < 240) {
		return PT_COLOR_new(0, x, C);
	}
	else if (h < 300) {
		return PT_COLOR_new(x, 0, C);
	}
	else { // 300 <= h < 360
		return PT_COLOR_new(C, 0, x);
	}
}

PT_COLOR PT_COLOR_new(float r, float g, float b) {
	return (PT_COLOR){ r, g, b };
}

float lerp(float x, float y, float a) {
	return (1 - a) * x + y * a;
}

// "methods"
PT_COLOR PT_COLOR_lerp(PT_COLOR c1, PT_COLOR c2, float alpha) {
	return PT_COLOR_new(
		lerp(c1.r, c2.r, alpha),
		lerp(c1.g, c2.g, alpha),
		lerp(c1.b, c2.b, alpha)
	);
}

PT_COLOR PT_COLOR_add(PT_COLOR c1, PT_COLOR c2) {
	return PT_COLOR_new(
		c1.r + c2.r,
		c1.g + c2.g,
		c1.b + c2.b
	);
}

PT_COLOR PT_COLOR_sub(PT_COLOR c1, PT_COLOR c2) {
	return PT_COLOR_new(
		c1.r - c2.r,
		c1.g - c2.g,
		c1.b - c2.b
	);
}

PT_COLOR PT_COLOR_mul(PT_COLOR c, float factor) {
	return PT_COLOR_new(
		c.r * factor,
		c.g * factor,
		c.b * factor
	);
}