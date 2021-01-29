#include "resource.h"

#include <cstdint>
#include <utility>

#include <Windows.h>

void smooth_wnd_show(const HWND wnd)
{
	for (std::uint16_t i{ 0u }; i <= 255u; i++)
	{
		::SetLayeredWindowAttributes(wnd, 0, i, LWA_ALPHA);
		::Sleep(1); // !!!
	}
	return;
}

void smooth_wnd_hide(
	const HWND wnd,
	const bool close)
{
	for (std::int16_t i{ 255u }; i > 0u; i--)
	{
		SetLayeredWindowAttributes(wnd, 0, i, LWA_ALPHA);
		Sleep(1); // !!!
	}

	if (close) 
	{
		::SendMessageW(wnd, WM_CLOSE, 0, 0);
		::DestroyWindow(wnd);
	}
}

::ATOM reg_wnd_class(
	::LPCWSTR class_name,
	::WNDPROC wnd_proc,
	::HINSTANCE inst)
{
	WNDCLASSEXW wc{};
	wc.cbClsExtra = NULL;
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.cbWndExtra = NULL;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(CreateSolidBrush(RGB(29, 29, 29)));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(APP_ICON));
	wc.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(APP_ICON));
	wc.hInstance = inst;
	wc.lpfnWndProc = wnd_proc;
	wc.lpszClassName = class_name;
	wc.lpszMenuName = NULL;
	wc.style = CS_DROPSHADOW;

	return ::RegisterClassExW(&wc);
}

std::pair<std::size_t, std::size_t> get_wnd_center(
	const std::size_t x,
	const std::size_t y)
{
	const std::size_t xc{ (::GetSystemMetrics(SM_CXSCREEN) - x) / 2u };
	const std::size_t yc{ (::GetSystemMetrics(SM_CYSCREEN) - y) / 2u };

	return std::make_pair(xc, yc);
}