#include "expandableArray.h"
#include <stdlib.h>
#include <string.h>

PT_EXPANDABLE_ARRAY PT_EXPANDABLE_ARRAY_new(int initialSize, int elementSizeBytes) {
	PT_EXPANDABLE_ARRAY arr = { 0 };
	
	arr.elementSize = elementSizeBytes;
	arr.data = calloc(initialSize, elementSizeBytes);
	arr.elementSpace = initialSize;
	arr.numElements = 0;

	return arr;
}

void PT_EXPANDABLE_ARRAY_destroy(PT_EXPANDABLE_ARRAY* arr) {
	free(arr->data);
}



void* PT_EXPANDABLE_ARRAY_get(PT_EXPANDABLE_ARRAY* arr, int i) {
	return arr->data + i * arr->elementSize;
}

void PT_EXPANDABLE_ARRAY_set(PT_EXPANDABLE_ARRAY* arr, int i, void* element) {
	memcpy(arr->data + i * arr->elementSize, element, arr->elementSize);
}

void accomodate_size(PT_EXPANDABLE_ARRAY* arr, int size) {
	int sizeChanged = 0;
	while (size > arr->elementSpace) {
		arr->elementSpace *= 2;
		sizeChanged = 1;
	}

	if (sizeChanged) {
		arr->data = realloc(arr->data, arr->elementSpace);
	}
}

void PT_EXPANDABLE_ARRAY_add(PT_EXPANDABLE_ARRAY* arr, void* element) {
	accomodate_size(arr, arr->numElements + 1);


	memcpy(
		arr->data + arr->numElements * arr->elementSize,
		element,
		arr->elementSize
		);

	arr->numElements++;
}

void PT_EXPANDABLE_ARRAY_insert(PT_EXPANDABLE_ARRAY* arr, int i, void* element) {
	accomodate_size(arr, arr->numElements + 1);

	int remainderSize = arr->numElements - i;
	if (remainderSize > 0) {
		char* src = arr->data + i * arr->elementSize;
		char* dest = src + arr->elementSize;
		
		char* srcCopy = calloc(remainderSize, arr->elementSize);
		memcpy(srcCopy, src, remainderSize * arr->elementSize);

		memcpy(dest, srcCopy, remainderSize * arr->elementSize);
		free(srcCopy);
	}

	memcpy(
		arr->data + i * arr->elementSize,
		element,
		arr->elementSize
	);
	arr->numElements++;
}

void PT_EXPANDABLE_ARRAY_remove(PT_EXPANDABLE_ARRAY* arr, int i) {
	int remainderSize = arr->numElements - i;
	
	if (remainderSize > 1) {
		char* temp = calloc(remainderSize - 1, arr->elementSize);
		memcpy(temp, arr->data + (i + 1) * arr->elementSize, (remainderSize - 1) * arr->elementSize);

		memcpy(arr->data + i * arr->elementSize, temp, (remainderSize - 1) * arr->elementSize);

		free(temp);
	}

	arr->numElements--;
}

int PT_EXPANDABLE_ARRAY_find(PT_EXPANDABLE_ARRAY* arr, void* element) {
	for (int i = 0; i < arr->numElements; i++) {
		void* cmp = arr->data + i * arr->elementSize;

		if (memcmp(cmp, element, arr->elementSize) == 0) {
			return i;
		}
	}

	return -1;
}