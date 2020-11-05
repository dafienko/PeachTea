#ifndef PT_SIZE_CONSTRAINT_H
#define PT_SIZE_CONSTRAINT_H

#include "vectorMath.h"
#include "ScreenDimension.h"

typedef enum {
	PTSC_LOCK_X,
	PTSC_LOCK_Y,
	PTSC_LOCK_BIGGEST,
	PTSC_LOCK_SMALLEST
} PTSC_AXIS;

typedef struct {
	vec2i minSize, maxSize;

	float aspectRatio;
	PTSC_AXIS lockedAxis;

	vec2i(*calculateSize)(struct PT_GUI_OBJ*, PT_ABS_DIM);
} PT_SIZE_CONSTRAINT;

#define NULL_BOUND (vec2i){-1, -1} // "unlimited" bound

PT_SIZE_CONSTRAINT* PT_SIZE_CONSTRAINT_none();
PT_SIZE_CONSTRAINT* PT_SIZE_CONSTRAINT_aspect(float aspectRatio, PTSC_AXIS lockedAxis);
PT_SIZE_CONSTRAINT* PT_SIZE_CONSTRAINT_bounds(vec2i min, vec2i max);

void PT_SIZE_CONSTRAINT_destroy(PT_SIZE_CONSTRAINT* ptsc);

#endif

