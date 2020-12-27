#include <string.h>
#include <stdlib.h>
#include "stringUtil.h"

typedef unsigned char byte;

void* clone_memory(void* source, int numBytes) {
	byte* clone = calloc(numBytes, 1);

	memcpy(clone, source, numBytes);

	return (void*)clone;
}

char* create_heap_str(const char* source) {
	return (char*)clone_memory(source, (strlen(source) + 1) * sizeof(char));
}

wchar_t* create_heap_wstr(const wchar_t* source) {
	return (wchar_t*)clone_memory(source, (strlen(source) + 1) * sizeof(wchar_t));
}