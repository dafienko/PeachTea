#define _CRT_SECURE_NO_WARNINGS

#include "errorUtil.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>
#include "glExtensions.h"

wchar_t* getStringFromVAList(const wchar_t* format, va_list* args) {
	long length = (wcslen(format) + 100) * 2;
	wchar_t* str = calloc(length, sizeof(wchar_t));

	vswprintf(str, length, format, *args);
	va_end(*args);

	return str;
}

void display_error_message(const wchar_t* title, const wchar_t* message) {
	MessageBox(NULL, message, title, MB_OK);
}

void error(const wchar_t* desc, ...) {
	va_list args;
	va_start(args, desc);
	wchar_t* formattedDesc = getStringFromVAList(desc, &args);

	display_error_message(L"Error", formattedDesc);

	free(formattedDesc);
}

void fatal_error(const wchar_t* desc, ...) {
	va_list args;
	va_start(args, desc);
	wchar_t* formattedDesc = getStringFromVAList(desc, &args);

	display_error_message(L"Fatal Error", formattedDesc);

	free(formattedDesc);
	exit(-1);
}

LPWSTR get_windows_error_text(int errNum) {
	LPWSTR lpMsgBuf;

	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		&lpMsgBuf,
		0, NULL);

	return lpMsgBuf;
}

void windows_error(int errNum, const wchar_t* desc, ...) {
	va_list args;
	va_start(args, desc);
	wchar_t* formattedDesc = getStringFromVAList(desc, &args);
	
	LPWSTR lpMsgBuf = get_windows_error_text(errNum);

	int len = wcslen(formattedDesc) + wcslen(lpMsgBuf) + 10;
	wchar_t* combinedBuf = calloc(len, sizeof(wchar_t));
	swprintf(combinedBuf, len, L"%s: %s", formattedDesc, lpMsgBuf);

	display_error_message(L"Error", combinedBuf);

	free(combinedBuf);
	free(formattedDesc);
    LocalFree(lpMsgBuf);
}

void fatal_windows_error(int errNum, const wchar_t* desc, ...) {
	va_list args;
	va_start(args, desc);
	wchar_t* formattedDesc = getStringFromVAList(desc, &args);

	LPWSTR lpMsgBuf = get_windows_error_text(errNum);

	int len = wcslen(formattedDesc) + wcslen(lpMsgBuf) + 10;
	wchar_t* combinedBuf = calloc(len, sizeof(wchar_t));
	swprintf(combinedBuf, len, L"%s: %s", formattedDesc, lpMsgBuf);

	display_error_message(L"Error", combinedBuf);

	free(combinedBuf);
	free(formattedDesc);
	LocalFree(lpMsgBuf);
	exit(errNum);
}

void get_gl_error(const GLenum e, wchar_t** errNameOut, wchar_t** errDescOut) {
	if (e != 0) {
		const wchar_t* errName = L"";
		const wchar_t* errDesc = L"";
		int errFound = 1;

		switch (e) {
		case GL_INVALID_ENUM:
			errName = L"GL_INVALID_ENUM";
			errDesc = L"An unacceptable value is specified for an enumerated argument";
			break;
		case GL_INVALID_VALUE:
			errName = L"GL_INVALID_VALUE";
			errDesc = L"A numeric argument is out of range";
			break;
		case GL_INVALID_OPERATION:
			errName = L"GL_INVALID_OPERATION";
			errDesc = L"The specified operation is not allowed in the current state";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			errName = L"GL_INVALID_FRAMEBUFFER_OPERATION";
			errDesc = L"The framebuffer object is not complete";
			break;
		case GL_OUT_OF_MEMORY:
			errName = L"GL_OUT_OF_MEMORY";
			errDesc = L"There is not enough memory left to execute the command";
			break;
		case GL_STACK_UNDERFLOW:
			errName = L"GL_STACK_UNDERFLOW";
			errDesc = L"An attempt has been made to perform an operation that would cause an internal stack to underflow";
			break;
		case GL_STACK_OVERFLOW:
			errName = L"GL_STACK_OVERFLOW";
			errDesc = L"An attempt has been made to perform an operation that would cause an internal stack to overflow";
			break;
		default:
			errFound = 0;
			break;
		}

		if (errFound) {
			int numChars = wcslen(errName) + 1;
			*errNameOut = calloc(numChars, sizeof(wchar_t));
			memcpy(*errNameOut, errName, numChars * sizeof(wchar_t));

			numChars = wcslen(errDesc) + 1;
			*errDescOut = calloc(numChars, sizeof(wchar_t));
			memcpy(*errDescOut, errDesc, numChars * sizeof(wchar_t));
		}
		else {
			*errNameOut = NULL;
			*errDescOut = NULL;
		}
	}
}

void gl_error(wchar_t* filename, int lineNum) {
	GLenum e = glGetError();
	if (e != 0) {
		wchar_t* errName;
		wchar_t* errDesc;
		get_gl_error(e, &errName, &errDesc);

		if (errName != NULL && errDesc != NULL) {
			int fullStrLen = wcslen(errDesc) + wcslen(errName) + 400;
			wchar_t* fullErrDesc = calloc(fullStrLen, sizeof(wchar_t));

			swprintf(fullErrDesc, fullStrLen, L"%s in %s at line %i: %s", errName, filename, lineNum, errDesc);
			//swprintf(fullErrDesc, fullStrLen, L"%s ta;lk Ts", L"a;lkfa", L"a;lskdjfa;");
			display_error_message(L"GL_ERROR", fullErrDesc);

			free(fullErrDesc);
			free(errName);
			free(errDesc);
		}
	}
}