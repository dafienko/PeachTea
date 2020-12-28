#ifndef PT_EXPANDABLE_ARRAY_H
#define PT_EXPANDABLE_ARRAY_H

typedef struct {
	char* data;

	int elementSize;
	int numElements;

	int elementSpace;
} PT_EXPANDABLE_ARRAY;

PT_EXPANDABLE_ARRAY PT_EXPANDABLE_ARRAY_new(int initialSize, int elementSizeBytes);
void PT_EXPANDABLE_ARRAY_destroy(PT_EXPANDABLE_ARRAY* arr);

void* PT_EXPANDABLE_ARRAY_get(PT_EXPANDABLE_ARRAY* arr, int i);
void PT_EXPANDABLE_ARRAY_add(PT_EXPANDABLE_ARRAY* arr, void* element);
void PT_EXPANDABLE_ARRAY_insert(PT_EXPANDABLE_ARRAY* arr, int i, void* element);
void PT_EXPANDABLE_ARRAY_remove(PT_EXPANDABLE_ARRAY* arr, int i);
int PT_EXPANDABLE_ARRAY_find(PT_EXPANDABLE_ARRAY* arr, void* element);

#endif