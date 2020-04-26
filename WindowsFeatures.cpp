#include "Main.h"

void smoothWindowApprearance(HWND hWindow, bool isClosing) {
	for (uint16_t i = 0; i <= 255; i++) {
		SetLayeredWindowAttributes(hWindow, 0, static_cast<BYTE>(i), LWA_ALPHA);
		Sleep(1);
	}

	if (isClosing) {
		SendMessage(hWindow, WM_CLOSE, NULL, NULL);
		DestroyWindow(hWindow);
	}
}

void smoothWindowHide(HWND hWindow, bool isClosing) {

	for (int16_t i = 255; i >= 0; i--) {
		SetLayeredWindowAttributes(hWindow, 0, static_cast<BYTE>(i), LWA_ALPHA);
		Sleep(1);
	}

	if (isClosing) {
		SendMessage(hWindow, WM_CLOSE, NULL, NULL);
		DestroyWindow(hWindow);
	}
}

ATOM registerWindowClass(LPCWSTR winClassName, WNDPROC windowProcedure, HINSTANCE hInstance)
{
	WNDCLASSEX wc{}; // структура окна

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(CreateSolidBrush(RGB(29, 29, 29)));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = winClassName;
	wc.lpfnWndProc = windowProcedure;
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(APP_ICON));
	wc.lpszMenuName = NULL;
	wc.style = CS_DROPSHADOW;

	return RegisterClassEx(&wc);
}

std::pair<uint16_t, uint16_t> getWindowCenterCoordinates(uint16_t winWidth, uint16_t winHeight)
{
	uint16_t x = static_cast<uint16_t>((GetSystemMetrics(SM_CXSCREEN) - winWidth) / 2);
	uint16_t y = static_cast<uint16_t>((GetSystemMetrics(SM_CYSCREEN) - winHeight) / 2);
	return std::make_pair(x, y);
}