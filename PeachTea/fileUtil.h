#ifndef FILEUTIL_H
#define FILUTIL_H

void get_file_lines(const char* filename, int* numLines, char*** lines, int** lengths);
void freeLines(const int numLines, char*** lines);

#endif // !FILEUTIL_H
