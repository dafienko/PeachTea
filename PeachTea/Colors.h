#ifndef PT_COLORS_H
#define PT_COLORS_H

typedef struct {
	float r, g, b;
} PT_COLOR;

PT_COLOR PT_COLOR_fromRGB(int r, int g, int b);
PT_COLOR PT_COLOR_fromHSV(float h, float s, float v); //  0 <= h <= 360;   0 <= s & h <= 1;
PT_COLOR PT_COLOR_new(float r, float g, float b);

PT_COLOR PT_COLOR_lerp(PT_COLOR c1, PT_COLOR c2, float alpha);
PT_COLOR PT_COLOR_add(PT_COLOR c1, PT_COLOR c2);
PT_COLOR PT_COLOR_sub(PT_COLOR c1, PT_COLOR c2);
PT_COLOR PT_COLOR_mul(PT_COLOR c, float factor);

#endif