#include "peachTime.h"
#include <sys/timeb.h>

struct timeb START = { 0 };

void PT_TIME_start() { // call once at start of program
	ftime(&START);
}

float PT_TIME_get() {
	struct timeb now;

	ftime(&now);

	float diff = (now.time - START.time) + (now.millitm - START.millitm) / 1000.0f;

	return diff;
}