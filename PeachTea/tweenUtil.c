#include "tweenUtil.h"
#include "expandableArray.h"
#include "peachTime.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

PT_EXPANDABLE_ARRAY activeTweens;

void PT_TWEEN_init() {
	activeTweens = PT_EXPANDABLE_ARRAY_new(10, sizeof(PT_TWEEN*));
}

void PT_TWEEN_update() {
	float t = PT_TIME_get();

	for (int i = activeTweens.numElements - 1; i >= 0; i--) {
		PT_TWEEN* tween = *(PT_TWEEN**)PT_EXPANDABLE_ARRAY_get(&activeTweens, i);
		float startTime = tween->startTime;

		float dt = t - startTime;
		dt = min(dt, tween->tweenConfig.duration);

		float alpha = 1.0f;
		if (tween->tweenConfig.duration > 0) {
			alpha = dt / tween->tweenConfig.duration;
		}

		tween->alphaToValueFunc(tween->initValue, tween->endValue, tween->valueTarget, alpha, tween->tweenConfig.type, tween->tweenConfig.direction);

		if (alpha >= 1.0f) {
			PT_EXPANDABLE_ARRAY_remove(&activeTweens, i);
		}
	}
}

void PT_TWEEN_play(PT_TWEEN* tween) {
	tween->startTime = PT_TIME_get();
	tween->initValue = memcpy(tween->initValue, tween->valueTarget, tween->elementSizeBytes);

	// if their is another tween playing with the same target pointer, stop that tween and override it with the new one
	for (int i = 0; i < activeTweens.numElements; i++) {
		PT_TWEEN* t = *(PT_TWEEN**)PT_EXPANDABLE_ARRAY_get(&activeTweens, i);
		if (t->valueTarget == tween->valueTarget) {
			PT_EXPANDABLE_ARRAY_set(&activeTweens, i, (void*)&tween);
			return;
		}
	}

	PT_EXPANDABLE_ARRAY_add(&activeTweens, (void*)&tween);
}

void PT_TWEEN_stop(PT_TWEEN* tween) {
	int index = PT_EXPANDABLE_ARRAY_find(&activeTweens, (void*)&tween);
	if (index >= 0) { // if the tween is actually playing
		PT_EXPANDABLE_ARRAY_remove(&activeTweens, index);
	}
}


float PT_TWEEN_float(float a, float b, float alpha, TWEEN_TYPE type, TWEEN_DIRECTION direction) {
	float value = 0;
	float i = alpha;

	if (direction == PT_INOUT) {
		i *= 2.0f;

		if (i > 1.0f) {
			i -= 2.0f;
		}
	}

	switch (type) {
	case PT_LINEAR:
		value = alpha;
		break;
	case PT_QUADRATIC:
		switch (direction) {
		case PT_IN:
			value = alpha * alpha;
			break;
		case PT_OUT:
			value = 1 - powf(alpha - 1, 2);
			break;
		case PT_INOUT:
			if (alpha <= .5f) {
				value = .5f * i * i;
			}
			else {
				value = -.5f * i * i + 1;
			}
			break;
		}
		break;
	case PT_CUBIC:
		switch (direction) {
		case PT_IN:
			value = alpha * alpha * alpha;
			break;
		case PT_OUT:
			value = powf(alpha - 1, 3.0f) + 1;
			break;
		case PT_INOUT:
			if (alpha <= .5f) {
				value = .5f * i * i * i;
			}
			else {
				value = .5f * i * i * i + 1;
			}
			break;
		}
		break;
	}

	return (1 - value) * a + b * value;
}

vec2f PT_TWEEN_vec2f(vec2f a, vec2f b, float alpha, TWEEN_TYPE type, TWEEN_DIRECTION direction) {
	return (vec2f) {PT_TWEEN_float(a.x, b.x, alpha, type, direction), PT_TWEEN_float(a.y, b.y, alpha, type, direction)};
}

PT_REL_DIM PT_TWEEN_PT_REL_DIM(PT_REL_DIM a, PT_REL_DIM b, float alpha, TWEEN_TYPE type, TWEEN_DIRECTION direction) {
	float xScale = PT_TWEEN_float(a.xFactor, b.xFactor, alpha, type, direction);
	float xOff = PT_TWEEN_float((float)a.xOffset, (float)b.xOffset, alpha, type, direction);
	float yScale = PT_TWEEN_float(a.yFactor, b.yFactor, alpha, type, direction);
	float yOff = PT_TWEEN_float((float)a.yOffset, (float)b.yOffset, alpha, type, direction);

	return PT_REL_DIM_new(xScale, (int)xOff, yScale, (int)yOff);
}

void intermediate_tween_float(void* a, void* b, void* dest, float alpha, TWEEN_TYPE type, TWEEN_DIRECTION direction) {
	float fa = *(float*)a;
	float fb = *(float*)b;
	float* fdest = (float*)dest;

	float v = PT_TWEEN_float(fa, fb, alpha, type, direction);
	*fdest = v;
}

void intermediate_tween_vec2f(void* a, void* b, void* dest, float alpha, TWEEN_TYPE type, TWEEN_DIRECTION direction) {
	vec2f fa = *(vec2f*)a;
	vec2f fb = *(vec2f*)b;
	vec2f* fdest = (vec2f*)dest;

	vec2f v = PT_TWEEN_vec2f(fa, fb, alpha, type, direction);
	*fdest = v;
}

void intermediate_tween_PT_REL_DIM(void* a, void* b, void* dest, float alpha, TWEEN_TYPE type, TWEEN_DIRECTION direction) {
	PT_REL_DIM fa = *(PT_REL_DIM*)a;
	PT_REL_DIM fb = *(PT_REL_DIM*)b;
	PT_REL_DIM* fdest = (PT_REL_DIM*)dest;

	PT_REL_DIM v = PT_TWEEN_PT_REL_DIM(fa, fb, alpha, type, direction);
	*fdest = v;
}

PT_TWEEN* tween_new(void* start, void* end, void* dest, int elementSizeBytes, TWEEN_CONFIG config) {
	PT_TWEEN* tween = calloc(1, sizeof(PT_TWEEN));

	tween->initValue = calloc(1, elementSizeBytes);
	tween->endValue = calloc(1, elementSizeBytes);

	memcpy(tween->initValue, start, elementSizeBytes);
	memcpy(tween->endValue, end, elementSizeBytes);

	tween->valueTarget = dest;
	
	tween->tweenConfig = config;

	return tween;
}

PT_TWEEN* PT_TWEEN_float_new(float end, float* dest, TWEEN_CONFIG config) {
	PT_TWEEN* tween = tween_new(dest, &end, dest, sizeof(float), config);
	
	tween->elementSizeBytes = sizeof(float);
	tween->alphaToValueFunc = intermediate_tween_float;

	return tween;
}

PT_TWEEN* PT_TWEEN_vec2f_new(vec2f end, vec2f* dest, TWEEN_CONFIG config) {
	PT_TWEEN* tween = tween_new(dest, &end, dest, sizeof(vec2f), config);

	tween->elementSizeBytes = sizeof(vec2f);
	tween->alphaToValueFunc = intermediate_tween_vec2f;

	return tween;
}

PT_TWEEN* PT_TWEEN_PT_REL_DIM_new(PT_REL_DIM end, PT_REL_DIM* dest, TWEEN_CONFIG config) {
	PT_TWEEN* tween = tween_new(dest, &end, dest, sizeof(PT_REL_DIM), config);

	tween->elementSizeBytes = sizeof(PT_REL_DIM);
	tween->alphaToValueFunc = intermediate_tween_PT_REL_DIM;

	return tween;
}


void PT_TWEEN_destroy(PT_TWEEN* tween) {
	PT_TWEEN_stop(tween); // stop the tween in case it's playing

	free(tween->initValue);
	free(tween->initValue);
	free(tween);
}