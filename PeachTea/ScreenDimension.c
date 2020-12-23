#include "ScreenDimension.h"
#include "vectorMath.h"
#include "clamp.h"

vec2i calculate_screen_dimension(PT_REL_DIM screenDimension, vec2i screenSize) {
	vec2i r = { 0 };

	switch (screenDimension.relativeAxes) {
	case PT_SA_XX:
		screenSize.y = screenSize.x;
		break;
	case PT_SA_YY:
		screenSize.x = screenSize.y;
		break;
	}

	r.x = (screenDimension.xFactor * screenSize.x) + screenDimension.xOffset;
	r.y = (screenDimension.yFactor * screenSize.y) + screenDimension.yOffset;

	return r;
}

PT_canvas calculate_child_canvas(PT_canvas parentCanvas, vec2i pos, vec2i size, vec2f anchorPoint, int clipDescendants) {
	vec2i parentPos = canvas_pos(parentCanvas);

	vec2i framePos = vector_add_2i(pos, parentPos);
	vec2i anchorOffset = (vec2i){
		anchorPoint.x * size.x,
		anchorPoint.y * size.y
	};
	framePos = vector_sub_2i(framePos, anchorOffset);

	PT_canvas canvas = { 0 };

	canvas.top = framePos.y;
	canvas.bottom = framePos.y + size.y;
	canvas.left = framePos.x;
	canvas.right = framePos.x + size.x;

	canvas.top -= parentCanvas.oy;
	canvas.bottom -= parentCanvas.oy;
	canvas.left -= parentCanvas.ox;
	canvas.right -= parentCanvas.ox;

	if (clipDescendants) {
		if (parentCanvas.clipDescendants) {
			canvas.ctop = clamp(canvas.top, parentCanvas.ctop, parentCanvas.cbottom);
			canvas.cbottom = clamp(canvas.bottom, parentCanvas.ctop, parentCanvas.cbottom);
			canvas.cright = clamp(canvas.right, parentCanvas.cleft, parentCanvas.cright);
			canvas.cleft = clamp(canvas.left, parentCanvas.cleft, parentCanvas.cright);
		}
		else {
			canvas.ctop = canvas.top;
			canvas.cbottom = canvas.bottom;
			canvas.cright = canvas.right;
			canvas.cleft = canvas.left;
		}
	}
	else {
		canvas.ctop = parentCanvas.ctop;
		canvas.cbottom = parentCanvas.cbottom;
		canvas.cright = parentCanvas.cright;
		canvas.cleft = parentCanvas.cleft;
	}

	canvas.clipDescendants = parentCanvas.clipDescendants || clipDescendants;

	return canvas;
}

PT_REL_DIM PT_REL_DIM_new(float xScale, int xOff, float yScale, int yOff) {
	PT_REL_DIM dim = { 0 };

	dim.xFactor = xScale;
	dim.xOffset = xOff;

	dim.yFactor = yScale;
	dim.yOffset = yOff;

	dim.relativeAxes = PT_SA_XY;

	return dim;
}