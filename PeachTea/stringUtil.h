#ifndef PT_STRING_UTIL_H
#define PT_STRING_UTIL_H

char* create_heap_str(const char* source);
wchar_t* create_heap_wstr(const wchar_t* source);

void free_lines(char** lines, int numlines);
void getStrLines(const char* str, char*** linesOut, int* numLinesOut);

#endif
