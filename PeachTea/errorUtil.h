#ifndef ERROR_UTIL_H
#define ERROR_UTIL_H

#include "lightWindows.h"

void error(const wchar_t* desc, ...);
void fatal_error(const wchar_t* desc, ...);

void windows_error(int errNum, const wchar_t* desc, ...);
void fatal_windows_error(int errNum, const wchar_t* desc, ...);

void gl_error(wchar_t* filename, int lineNum);

#define CHECK_GL_ERRORS gl_error(__FILEW__, __LINE__)

#endif