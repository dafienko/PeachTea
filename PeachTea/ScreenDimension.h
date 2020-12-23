#ifndef PT_DIMENSIONS_H
#define PT_DIMENSIONS_H

#include "vectorMath.h"
#include "canvas.h"

typedef struct {
	vec2i position;
	vec2i size;

	float depth; // not always used, but really convenient to put here for ui rendering
	enum Z_SORTING_TYPE sortingType;
} PT_ABS_DIM;

typedef enum {
	PT_SA_XY,
	PT_SA_XX,
	PT_SA_YY
} PT_SCREEN_AXIS;

typedef struct {
	float xFactor;
	int xOffset;

	float yFactor;
	int yOffset;

	PT_SCREEN_AXIS relativeAxes;
} PT_REL_DIM;

vec2i calculate_screen_dimension(PT_REL_DIM screenDimension, vec2i screenSize);
PT_canvas calculate_child_canvas(PT_canvas parentCanvas, vec2i pos, vec2i size, vec2f anchorPoint, int clipDescendants);

PT_REL_DIM PT_REL_DIM_new(float xScale, int xOff, float yScale, int yOff);

#endif