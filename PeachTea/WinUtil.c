#include "WinUtil.h"
#include "errorUtil.h"

int winUtilInitialized = 0;

WCHAR* MAIN_WNDCLASS_CLASSNAME = L"PeachTea_MAIN";
WNDCLASS MAIN_WNDCLASS = { 0 };
PIXELFORMATDESCRIPTOR pfd = { 0 };

RECT desktopRect;
POINT defPosition, defSize;

void initWinUtil(const HINSTANCE hInstance) {
	if (!winUtilInitialized) {
		winUtilInitialized = 1;

		// calculate default window position and dimensions
		defPosition = (POINT){ 0 };
		defSize = (POINT){ 0 };
		desktopRect = (RECT){ 0 };

		desktopRect.right = GetSystemMetrics(SM_CXSCREEN);
		desktopRect.bottom = GetSystemMetrics(SM_CYSCREEN);

		defSize = (POINT){ 800, 600 };

		defPosition = (POINT){ 
			(R_WIDTH(desktopRect) - defSize.x) / 2,
			(R_HEIGHT(desktopRect) - defSize.y) / 2,
		} ;

		// define and register main window class
		MAIN_WNDCLASS.lpfnWndProc = DefWindowProc;
		MAIN_WNDCLASS.hInstance = hInstance;
		MAIN_WNDCLASS.lpszClassName = MAIN_WNDCLASS_CLASSNAME;
		MAIN_WNDCLASS.hCursor = NULL;
		MAIN_WNDCLASS.hbrBackground = GetStockObject(DKGRAY_BRUSH);
		MAIN_WNDCLASS.style = CS_OWNDC;
		MAIN_WNDCLASS.cbClsExtra = 0;
		MAIN_WNDCLASS.cbWndExtra = 0;
		MAIN_WNDCLASS.lpszMenuName = NULL;
		MAIN_WNDCLASS.hIcon = NULL;


		// define opengl pixel format descriptor
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
	}
}

HWND createPeachWindow(const HINSTANCE hInstance, const void* wndProc, const WCHAR* wndName) {
	if (!winUtilInitialized) {
		initWinUtil(hInstance);
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
	HWND hWnd = CreateWindow(wndName, wndName,
		WS_OVERLAPPEDWINDOW,
		defPosition.x, defPosition.y,
		defSize.x, defSize.y,
		NULL, NULL, hInstance, NULL
	);

	if (hWnd == NULL) {
		int e = GetLastError();
		
		fatal_windows_error(e, L"CreateWindow fucked up in %s at line %i", __FILEW__, __LINE__);
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