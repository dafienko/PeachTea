#define _CRT_SECURE_NO_WARNINGS

#include "expandableArrayTests.h"
#include "PeachTea.h"
#include <stdio.h>
#include <stdlib.h>

void print_arr(PT_EXPANDABLE_ARRAY arr) {
	char* buffer = calloc(2000, sizeof(char));
	int bufferIndex = 0;
	for (int i = 0; i < arr.numElements; i++) {
		int x = *(int*)PT_EXPANDABLE_ARRAY_get(&arr, i);
		char* str = calloc(10, sizeof(char));
		if (i < arr.numElements - 1) {
			sprintf(str, "%i, ", x);
		}
		else {
			sprintf(str, "%i", x);
		}
		memcpy(buffer + bufferIndex, str, strlen(str) * sizeof(char));
		bufferIndex += strlen(str);
		free(str);
	}

	printf("%s\n", buffer);
	
	free(buffer);
}

void test_expandable_arrays() {
	PT_EXPANDABLE_ARRAY arr = PT_EXPANDABLE_ARRAY_new(10, sizeof(int));
	int numbers[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	for (int i = 0; i < 10; i++) {
		PT_EXPANDABLE_ARRAY_add(&arr, &numbers[i]);
	}
	print_arr(arr);
	

	printf("\n\nremoving indices 0, 5, and 9...\n");
	PT_EXPANDABLE_ARRAY_remove(&arr, 0);
	print_arr(arr);
	PT_EXPANDABLE_ARRAY_remove(&arr, 5);
	print_arr(arr);
	PT_EXPANDABLE_ARRAY_remove(&arr, 9);
	print_arr(arr);
	
	printf("\n\ninserting 69 to index 0 and 420 to index 8\n");
	int n = 69;
	PT_EXPANDABLE_ARRAY_insert(&arr, 0, &n);
	print_arr(arr);
	n = 420;
	PT_EXPANDABLE_ARRAY_insert(&arr, 8, &n);
	print_arr(arr);


	printf("\n\nhit enter create a big array...\n");
	while (getchar()) {
		PT_EXPANDABLE_ARRAY_destroy(&arr);
		arr = PT_EXPANDABLE_ARRAY_new(100, sizeof(int));

		for (int i = 0; i < 250000; i++) {
			PT_EXPANDABLE_ARRAY_add(&arr, &i);
		}
		printf("created a 250,000 elementa array\n");
	}
}