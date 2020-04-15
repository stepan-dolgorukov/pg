#include "Main.h"

void smoothWindowApprearance(HWND hWnd, bool isClosing) {
	for (uint16_t i = 0; i <= 255; i++) {
		SetLayeredWindowAttributes(hWnd, 0, static_cast<unsigned char>(i), LWA_ALPHA);
		Sleep(1);
	}

	if (isClosing) {
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		DestroyWindow(hWnd);
	}
}

void smoothWindowHide(HWND hWnd, bool isClosing) {

	for (int16_t i = 255; i >= 0; i--) {
		SetLayeredWindowAttributes(hWnd, 0, static_cast<unsigned char>(i), LWA_ALPHA);
		Sleep(1);
	}

	if (isClosing) {
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		DestroyWindow(hWnd);
	}
}

ATOM registerWindowClass(const wchar_t* winClassName, WNDPROC windowProcedure, HINSTANCE hInstance)
{
	WNDCLASSEX windowClass{ 0 }; // структура окна

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.hInstance = hInstance;
	windowClass.hbrBackground = reinterpret_cast<HBRUSH>(CreateSolidBrush(RGB(29, 29, 29)));
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = winClassName;
	windowClass.lpfnWndProc = windowProcedure;
	windowClass.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(APP_ICON));
	windowClass.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(APP_ICON));
	windowClass.lpszMenuName = NULL;
	windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_DROPSHADOW;

	return RegisterClassEx(&windowClass);
}

std::pair<uint16_t, uint16_t> getWindowCenterCoordinates(uint16_t winWidth, uint16_t winHeight)
{
	uint16_t x = static_cast<uint16_t>((GetSystemMetrics(SM_CXSCREEN) - winWidth) / 2);
	uint16_t y = static_cast<uint16_t>((GetSystemMetrics(SM_CYSCREEN) - winHeight) / 2);
	return std::make_pair(x, y);
}