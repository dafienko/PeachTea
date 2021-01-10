#ifndef TWEEN_UTIL_H
#define TWEEN_UTIL_H

#include "vectorMath.h"
#include "matrixMath.h"
#include "Colors.h"
#include "ScreenDimension.h"

typedef enum {
	PT_LINEAR,
	PT_QUADRATIC,
	PT_CUBIC,
} TWEEN_TYPE;

typedef enum {
	PT_IN,
	PT_INOUT,
	PT_OUT
} TWEEN_DIRECTION;

typedef struct {
	TWEEN_TYPE type;
	TWEEN_DIRECTION direction;
	float duration;
} TWEEN_CONFIG;

typedef struct {
	TWEEN_CONFIG tweenConfig;
	
	int elementSizeBytes;

	float startTime;

	void* initValue;
	void* endValue;

	void* valueTarget;

	void (*alphaToValueFunc)(void*, void*, void*, float, TWEEN_TYPE, TWEEN_DIRECTION);
} PT_TWEEN;

void PT_TWEEN_init();
void PT_TWEEN_update();

void PT_TWEEN_play(PT_TWEEN* tween);
void PT_TWEEN_stop(PT_TWEEN* tween);

float PT_TWEEN_float(float a, float b, float alpha, TWEEN_TYPE type, TWEEN_DIRECTION direction);
vec2f PT_TWEEN_vec2f(vec2f a, vec2f b, float alpha, TWEEN_TYPE type, TWEEN_DIRECTION direction);
PT_REL_DIM PT_TWEEN_PT_REL_DIM(PT_REL_DIM a, PT_REL_DIM b, float alpha, TWEEN_TYPE type, TWEEN_DIRECTION direction);

PT_TWEEN* PT_TWEEN_float_new(float end, float* dest, TWEEN_CONFIG config);
PT_TWEEN* PT_TWEEN_vec2f_new(vec2f end, vec2f* dest, TWEEN_CONFIG config);
PT_TWEEN* PT_TWEEN_PT_REL_DIM_new(PT_REL_DIM end, PT_REL_DIM* dest, TWEEN_CONFIG config);


void PT_TWEEN_destroy(PT_TWEEN* tween);

#endif