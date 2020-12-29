#include "canvas.h"
#include "clamp.h"

vec2i canvas_size(PT_canvas canvas) {
	return (vec2i) { CANVAS_WIDTH(canvas), CANVAS_HEIGHT(canvas) };
}

vec2i canvas_pos(PT_canvas canvas) {
	return (vec2i) { canvas.left, canvas.top > canvas.bottom ? canvas.bottom : canvas.top };
}


int child_canvas_in_parent_canvas(PT_canvas childCanvas, PT_canvas parentCanvas) {
	int parentUpper = MAX(parentCanvas.top, parentCanvas.bottom); // depending on coordinate system,
	int parentLower = MIN(parentCanvas.top, parentCanvas.bottom); // sometimes top > bottom, and sometimes bottom > top. this eliminates ambiguity

	int topInCanvas = childCanvas.top >= parentLower && childCanvas.top <= parentUpper;
	int bottomInCanvas = childCanvas.bottom >= parentLower && childCanvas.bottom <= parentUpper;

	int rightInCanvas = childCanvas.right >= parentCanvas.left && childCanvas.right <= parentCanvas.right;
	int leftInCanvas = childCanvas.left >= parentCanvas.left && childCanvas.left <= parentCanvas.right;

	return (topInCanvas || bottomInCanvas) && (rightInCanvas || leftInCanvas);
}