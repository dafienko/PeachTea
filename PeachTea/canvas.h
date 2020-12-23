#ifndef CANVAS_H
#define CANVAS_H

#include <math.h>
#include "vectorMath.h"

typedef struct {
	int right, left, top, bottom;

	int clipDescendants;
	int cright, cleft, ctop, cbottom; // clipping bounds

	int ox, oy; // internal canvas offsets
} PT_canvas;

#define CANVAS_WIDTH(canvas) canvas.right - canvas.left
// top may be > bottom or vice versa depending on context, height should be absolute value
#define CANVAS_HEIGHT(canvas) abs(canvas.top - canvas.bottom) 

vec2i canvas_size(PT_canvas canvas);
vec2i canvas_pos(PT_canvas canvas);

int child_canvas_in_parent_canvas(PT_canvas childCanvas, PT_canvas parentCanvas);

#endif