#ifndef SORT_H
#define SORT_H

typedef char byte;

void quicksort(byte* arr, int elementSizeBytes, int numElements, int(*cmp)(void*, void*));

#endif