#include "PeachTea.h"
#include "screenSize.h"
#include "renderer.h"

HDC hMainDC;
HGLRC hMainRC;
HWND hMainWnd = NULL;

int mainProgramLoop();

void PT_INIT(vec2i screenSize) {
	screensize_init(screenSize);
	GLEInit();
	init_gui_util();
	initFT();

	renderer_init();
}

void update_mouse_pos() {
	POINT p = { 0 };
	GetCursorPos(&p);

	mousePos = (vec2i){ p.x, p.y };
}

void PT_UPDATE() {
	update_mouse_pos();
}

LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void PT_CREATE_MAIN_WND(vec2i size, const char* title) {	
	hMainWnd = createPeachWindow(NULL, (void*)WndProc, L"Peach Tea");

	RECT r = { 0 };
	GetClientRect(hMainWnd, &r);
	vec2i screenSize = (vec2i){ r.right - r.left, r.bottom - r.top };

	hMainDC = GetDC(hMainWnd);
	hMainRC = wglCreateContext(hMainDC);
	wglMakeCurrent(hMainDC, hMainRC);

	PT_INIT(screenSize);
}

void PT_GET_MAIN_HWND() {
	return hMainWnd;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static PAINTSTRUCT ps;

	switch (uMsg) {
	case WM_SIZE:
		;
		vec2i size = (vec2i){ LOWORD(lParam), HIWORD(lParam) };
		PT_RESIZE(size);
		renderer_resized();

		render();
		return 0;

	case WM_CLOSE:
		PostQuitMessage(69);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int mainProgramLoop() {
	static MSG msg;
	int exitCode = 0;

	while (1) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				exitCode = msg.wParam;
				return;
			}
		}

		PT_UPDATE();
		render();
	}


	wglMakeCurrent(NULL, NULL);
	ReleaseDC(hMainWnd, hMainDC);
	wglDeleteContext(hMainRC);

	return exitCode;
}

int PT_RUN() {
	return mainProgramLoop();
}










