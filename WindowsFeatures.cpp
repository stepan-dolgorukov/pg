#include "Main.h"

void smoothWindowApprearance(HWND hWindow) {

	for (BYTE i = 0; i < 255; i++) 
	{
		SetLayeredWindowAttributes(hWindow, 0, i, LWA_ALPHA);
		Sleep(1);
	}

	SetLayeredWindowAttributes(hWindow, 0, 255, LWA_ALPHA);
}

void smoothWindowHide(HWND hWindow, bool isClosing) {

	for (BYTE i = 255; i > 0; i--) {
		SetLayeredWindowAttributes(hWindow, 0, i, LWA_ALPHA);
		Sleep(1);
	}

	SetLayeredWindowAttributes(hWindow, 0, 0, LWA_ALPHA);

	if (isClosing) 
	{
		SendMessage(hWindow, WM_CLOSE, NULL, NULL);
		DestroyWindow(hWindow);
	}
}

ATOM registerWindowClass(LPCWSTR winClassName, WNDPROC windowProcedure, HINSTANCE hInstance)
{
	WNDCLASSEXW wc;
	wc.cbClsExtra = NULL;
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.cbWndExtra = NULL;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(CreateSolidBrush(RGB(29, 29, 29)));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(APP_ICON));
	wc.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(APP_ICON));
	wc.hInstance = hInstance;
	wc.lpfnWndProc = windowProcedure;
	wc.lpszClassName = winClassName;
	wc.lpszMenuName = NULL;
	wc.style = CS_DROPSHADOW;

	return RegisterClassEx(&wc);
}

std::pair<int32_t, int32_t> getWindowCenterCoordinates(uint32_t uWidth, uint32_t uHeight)
{
	int32_t x = (GetSystemMetrics(SM_CXSCREEN) - uWidth) / 2;
	int32_t y = (GetSystemMetrics(SM_CYSCREEN) - uHeight) / 2;
	return std::make_pair(x, y);
}