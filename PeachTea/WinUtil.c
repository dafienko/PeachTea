#include "WinUtil.h"
#include "errorUtil.h"
#include "vectorMath.h"

int winUtilInitialized = 0;

WCHAR* MAIN_WNDCLASS_CLASSNAME = L"PeachTea_MAIN";
WNDCLASS MAIN_WNDCLASS = { 0 };
PIXELFORMATDESCRIPTOR pfd = { 0 };

RECT desktopRect;
vec2i defSize;

vec2i get_position_for_size(vec2i size) {
	return (vec2i){
		(R_WIDTH(desktopRect) - size.x) / 2,
		(R_HEIGHT(desktopRect) - size.y) / 2,
	};
}

void initWinUtil(const HINSTANCE hInstance) {
	if (!winUtilInitialized) {
		winUtilInitialized = 1;

		// calculate default window position and dimensions
		desktopRect.right = GetSystemMetrics(SM_CXSCREEN);
		desktopRect.bottom = GetSystemMetrics(SM_CYSCREEN);

		defSize = (vec2i){ 800, 600 };

		// define and register main window class
		MAIN_WNDCLASS.lpfnWndProc = DefWindowProc;
		MAIN_WNDCLASS.hInstance = hInstance;
		MAIN_WNDCLASS.lpszClassName = MAIN_WNDCLASS_CLASSNAME;
		MAIN_WNDCLASS.hCursor = LoadCursor(NULL, IDC_ARROW);
		MAIN_WNDCLASS.hbrBackground = GetStockObject(DKGRAY_BRUSH);
		MAIN_WNDCLASS.style = CS_OWNDC;
		MAIN_WNDCLASS.cbClsExtra = 0;
		MAIN_WNDCLASS.cbWndExtra = 0;
		MAIN_WNDCLASS.lpszMenuName = NULL;
		MAIN_WNDCLASS.hIcon = NULL;


		// define opengl pixel format descriptor
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
	}
}

HWND createPeachWindow(const HINSTANCE hInstance, vec2i wndSize, const void* wndProc, const WCHAR* wndName) {
	if (!winUtilInitialized) {
		initWinUtil(hInstance);
	}

	if (wndSize.x == 0 && wndSize.y == 0) {
		wndSize = (vec2i){ defSize.x, defSize.y };
	}
	else {
		wndSize.y += GetSystemMetrics(SM_CYCAPTION);
	}

	// clone default class
	WNDCLASS wndClass = { 0 };
	memcpy(&wndClass, &MAIN_WNDCLASS, sizeof(wndClass));
	if (wndProc != NULL) {
		wndClass.lpfnWndProc = wndProc;
	}
	wndClass.lpszClassName = wndName;

	// register 'tailored' wndclass
	int e = RegisterClass(&wndClass);
	if (e == 0) {
		e = GetLastError();

		fatal_windows_error(e, L"init failed (RegisterClass): %s line %i", __FILEW__, __LINE__);
	}

	// create window with custom wndclass
	vec2i wndPos = get_position_for_size(wndSize);

	HWND hWnd = CreateWindow(wndName, wndName,
		WS_OVERLAPPEDWINDOW,
		wndPos.x, wndPos.y,
		wndSize.x, wndSize.y,
		NULL, NULL, hInstance, NULL
	);

	if (hWnd == NULL) {
		int e = GetLastError();
		
		fatal_windows_error(e, L"CreateWindow returned NULL up in %s at line %i", __FILEW__, __LINE__);
	}

	HDC hdc = GetDC(hWnd);
	
	int pf = ChoosePixelFormat(hdc, &pfd);
	if (pf == 0) {
		int e = GetLastError();
		fatal_windows_error(e, L"ChoosePixelFormat choked in %s at line %i", __FILEW__, __LINE__);
	}

	if (SetPixelFormat(hdc, pf, &pfd) == FALSE) {
		int e = GetLastError();
		fatal_windows_error(e, L"SetPixelFormat failed in %s at line %i", __FILEW__, __LINE__);
	}

	DescribePixelFormat(hdc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	ReleaseDC(hWnd, hdc);

	UpdateWindow(hWnd);
	ShowWindow(hWnd, SW_SHOW);

	return hWnd;
}