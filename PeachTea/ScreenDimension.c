#include "ScreenDimension.h"
#include "vectorMath.h"

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

PT_REL_DIM PT_REL_DIM_new(float xScale, int xOff, float yScale, int yOff) {
	PT_REL_DIM dim = { 0 };

	dim.xFactor = xScale;
	dim.xOffset = xOff;

	dim.yFactor = yScale;
	dim.yOffset = yOff;

	dim.relativeAxes = PT_SA_XY;

	return dim;
}