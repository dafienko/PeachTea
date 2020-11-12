#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;

void swap_elements(byte* arr, int elementSizeBytes, int i, int j) {
	byte* p = arr + i * elementSizeBytes;
	byte* q = arr + j * elementSizeBytes;

	byte tmp;

	for (int i = 0; i != elementSizeBytes; ++i)
	{
		tmp = p[i];
		p[i] = q[i];
		q[i] = tmp;
	}
}

/*

comparison function returns
	1	if a > b
	-1	if a < b
	0	if a == b
*/

int quicksort_partition_with_pivot(byte* arr, int elementSizeBytes, int(*cmp)(void*, void*), int start, int end, byte* pivot) {
	int leftIndex = start;
	int rightIndex = end;

	byte* leftItem = arr + leftIndex * elementSizeBytes;
	byte* rightItem = arr + rightIndex * elementSizeBytes;

	while (leftIndex != end && cmp(leftItem, pivot) <= 0) {
		leftIndex++;
		leftItem = arr + leftIndex * elementSizeBytes;
	}

	while (rightIndex != start && cmp(rightItem, pivot) > 0) {
		rightIndex--;
		rightItem = arr + rightIndex * elementSizeBytes;
	}

	if (leftIndex + 1 <= rightIndex) {
		swap_elements(arr, elementSizeBytes, leftIndex, rightIndex);
		return quicksort_partition_with_pivot(arr, elementSizeBytes, cmp, leftIndex + 1, rightIndex - 1, pivot);
	}
	else { // leftIndex >= rightIndex
		int r = cmp(leftItem, pivot);

		if (r == 1) {
			return leftIndex;
		}
		else {
			return end + 1;
		}

		return leftIndex;
	}
}

void quicksort_partition(byte* arr, int elementSizeBytes, int numElements, int(*cmp)(void*, void*), int start, int end) {
	if (start >= end) {
		return;
	}

	byte* pivot = arr + end * elementSizeBytes;
	int pivotSwapIndex = -1;
	while (pivotSwapIndex < 0) {
		pivotSwapIndex = quicksort_partition_with_pivot(arr, elementSizeBytes, cmp, start, end - 1, pivot);
	}

	swap_elements(arr, elementSizeBytes, end, pivotSwapIndex);

	quicksort_partition(arr, elementSizeBytes, numElements, cmp, start, pivotSwapIndex - 1);
	quicksort_partition(arr, elementSizeBytes, numElements, cmp, pivotSwapIndex + 1, end);
}

void quicksort(byte* arr, int elementSizeBytes, int numElements, int(*cmp)(void*, void*)) {
	quicksort_partition((byte*)arr, elementSizeBytes, numElements, cmp, 0, numElements - 1);
}


































