#include <stdio.h>
#include <stdlib.h>
#include "PeachTea.h"

int mainProgramLoop();

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main() {
	PT_CREATE_MAIN_WND((vec2i) { 800, 600 }, "PT Main Window");

	int exitCode = PT_RUN();

	return exitCode;
}


