#ifndef SORT_H
#define SORT_H

void quicksort(void* arr, int elementSizeBytes, int numElements, int(*cmp)(void*, void*));

#endif