#include "canvas.h"

vec2i canvas_size(PT_canvas canvas) {
	return (vec2i) { CANVAS_WIDTH(canvas), CANVAS_HEIGHT(canvas) };
}

vec2i canvas_pos(PT_canvas canvas) {
	return (vec2i) { canvas.left, canvas.top > canvas.bottom ? canvas.bottom : canvas.top };
}

// assumes top is < bottom
int child_canvas_in_parent_canvas(PT_canvas childCanvas, PT_canvas parentCanvas) {
	int topInCanvas = childCanvas.top > parentCanvas.top && childCanvas.top < parentCanvas.bottom;
	int bottomInCanvas = childCanvas.bottom > parentCanvas.top && childCanvas.bottom < parentCanvas.bottom;

	int rightInCanvas = childCanvas.right > parentCanvas.left && childCanvas.right < parentCanvas.right;
	int leftInCanvas = childCanvas.left > parentCanvas.left && childCanvas.left < parentCanvas.right;

	return (topInCanvas || bottomInCanvas) && (rightInCanvas || leftInCanvas);
}