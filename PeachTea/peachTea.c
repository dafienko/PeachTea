#define _CRT_SECURE_NO_WARNINGS

#include "PeachTea.h"
#include "screenSize.h"
#include "renderer.h"
#include "PeachTeaShaders.h"
#include <dwmapi.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/timeb.h>

#pragma comment(lib, "dwmapi")

HDC hMainDC = NULL;
HGLRC hMainRC = NULL;
HWND hMainWnd = NULL;

void(*renderCallback)(void);

int mainProgramLoop(void(*updateCallback)(float), void(*renderCB)(void));

int PT_window_cursor = IDC_ARROW;
void PT_set_window_cursor(int idc) {
	PT_window_cursor = idc;
}

int mousewheelDelta = 0;
int get_mousewheel_delta() {
	return mousewheelDelta;
}

void update_main_window_pos() {
	RECT rect = { 0 };

	vec2i clientSize = { 0 };
	GetClientRect(hMainWnd, &rect);
	clientSize = (vec2i){ rect.right, rect.bottom };

	vec2i windowSize = { 0 };
	GetWindowRect(hMainWnd, &rect);
	windowSize = (vec2i){ rect.right - rect.left, rect.bottom - rect.top };

	int leftBorderSize = (windowSize.x - clientSize.x) / 2;
	int topBorderSize = windowSize.y + -clientSize.y + -leftBorderSize;

	mainWindowPosition = (vec2i){ rect.left + leftBorderSize, rect.top + topBorderSize };
}

void PT_INIT(vec2i screenSize) {
	screensize_init(screenSize);
	GLEInit();
	initFT();
	PT_SHADERS_init();
	PT_TIME_start();
	PT_TWEEN_init();

	renderer_init();

	PT_SCREEN_UI_init();

	update_main_window_pos();
}

float lastTime = 0;
float PT_UPDATE() {
	float time = PT_TIME_get();

	float diff = time - lastTime;

	lastTime = time;

	PT_TWEEN_update();

	return diff;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void update_accent_color() {
	DWORD c = 0u;
	BOOL bool = 0;
	HRESULT hr = DwmGetColorizationColor(&c, &bool);
	unsigned char a = c >> 24;
	unsigned char r = (c >> 16) & 255u;
	unsigned char g = (c >> 8) & 255u;
	unsigned char b = (c >> 0) & 255u;

	accentColor = PT_COLOR_fromRGB(r, g, b);
}

void PT_CREATE_MAIN_WND(vec2i size, const char* title) {	
	update_accent_color();

	int len = strlen(title) + 2;
	wchar_t* wtitle = calloc(len, sizeof(wchar_t));
	mbstowcs(wtitle, title, len);
	hMainWnd = createPeachWindow(NULL, size, (void*)WndProc, wtitle);
	free(wtitle);

	RECT rect;
	GetClientRect(hMainWnd, &rect);
	screenSize = (vec2i){ rect.right - rect.left, rect.bottom - rect.top };

	mouse_init(hMainWnd);

	hMainDC = GetDC(hMainWnd);
	hMainRC = wglCreateContext(hMainDC);
	wglMakeCurrent(hMainDC, hMainRC);

	PT_INIT(screenSize);
}

HWND PT_GET_MAIN_HWND() {
	return hMainWnd;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	switch (uMsg) {
	case WM_SIZE:
		;
		vec2i size = (vec2i){ LOWORD(lParam), HIWORD(lParam) };
		PT_RESIZE(size); // update screenSize;
		update_main_window_pos(); // update window position
		renderer_resized(); // update glViewport
		render(renderCallback); 

		return 0;
	case WM_CLOSE:
		PostQuitMessage(69);
		break;
	case WM_MOVING:
		if (hWnd == hMainWnd) {
			update_main_window_pos();
		}
		break;
	case WM_INPUT:
		;
		UINT dwSize = sizeof(RAWINPUT);
		static BYTE lpb[sizeof(RAWINPUT)];

		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

		RAWINPUT* raw = (RAWINPUT*)lpb;

		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			mouse_input(raw);
		}
		break;
	case WM_LBUTTONDOWN:
		SetCapture(hMainWnd);
		PT_BINDABLE_EVENT_fire(&e_mouse1Down, NULL);
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		PT_BINDABLE_EVENT_fire(&e_mouse1Up, NULL);
		break;
	case WM_SETCURSOR:
		;
		int ncHitVal = LOWORD(lParam);
		if (ncHitVal == HTCLIENT) {
			//printf("%i\n", PT_window_cursor);
			SetCursor(LoadCursor(NULL, PT_window_cursor));
		}
		else {
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		break;
	case WM_MOUSEWHEEL:
		;
		int d = -GET_WHEEL_DELTA_WPARAM(wParam);
		mousewheelDelta = d;

		if (d > 0) {
			PT_BINDABLE_EVENT_fire(&e_wheelUp, &d);
		}
		else {
			PT_BINDABLE_EVENT_fire(&e_wheelDown, &d);
		}

		break;
	case WM_CHAR:
		;
		char c = wParam == '\r' ? '\n' : wParam; // convert '\r' to '\n'
		PT_BINDABLE_EVENT_fire(&eOnCharTyped, (void*)&c);
		break;
	case WM_KEYDOWN:
		if (is_key_down(VK_LCONTROL)) {
			int command = -1;
			switch(wParam) {
			case 'V':
				command = PT_PASTE;
				break;
			case 'X':
				command = PT_CUT;
				break;
			case 'C':
				command = PT_COPY;
				break;
			case 'A':
				command = PT_SELECTALL;
				break;
			}

			if (command >= 0) {
				PT_BINDABLE_EVENT_fire(&eOnCommand, &command);
			}
		}
		else {
			PT_BINDABLE_EVENT_fire(&eOnKeyPress, (void*)&wParam);
		}
		break;
	case WM_KEYUP:
		PT_BINDABLE_EVENT_fire(&eOnKeyRelease, (void*)&wParam);
		break;
	case WM_SYSKEYDOWN:
		PT_BINDABLE_EVENT_fire(&eOnSysKeyPress, (void*)&wParam);
		break;
	case WM_SYSKEYUP:
		PT_BINDABLE_EVENT_fire(&eOnSysKeyRelease, (void*)&wParam);
		break;
	case WM_GETMINMAXINFO: // set minimum size for window
		;
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = 350;
		lpMMI->ptMinTrackSize.y = 350;
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

int mainProgramLoop(void(*updateCallback)(float), void(*renderCB)(void)) {
	static MSG msg;
	int exitCode = 0;

	renderCallback = renderCB;

	PT_TIME_get(&lastTime);

	PT_FRAMETEXTURE ft = PT_FRAMETEXTURE_new(screenSize.x, screenSize.y, 0);

	while (1) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				exitCode = msg.wParam;
				return;
			}
		}

		float dt = PT_UPDATE(); 
		if (updateCallback) {
			updateCallback(dt);
		}

		render(renderCallback); 
	}


	wglMakeCurrent(NULL, NULL);
	ReleaseDC(hMainWnd, hMainDC);
	wglDeleteContext(hMainRC);

	return exitCode;
}

int PT_RUN(void(*updateCallback)(float), void(*renderCallback)(void)) {
	PT_FRAMETEXTURE_new(screenSize.x, screenSize.y, 0);

	return mainProgramLoop(updateCallback, renderCallback);
}










