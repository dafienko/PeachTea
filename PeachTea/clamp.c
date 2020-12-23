#include "clamp.h"

int clamp(int x, int a, int b) {
	return MIN(b, MAX(a, x));
}