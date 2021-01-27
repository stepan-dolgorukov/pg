#include "control_btn.h"

#include <Windows.h>
#include <CommCtrl.h> // DefSubclassProc

::LRESULT __stdcall control_btn_proc(
	::HWND wnd,
	::UINT msg,
	::WPARAM wparam,
	::LPARAM lparam,
	::UINT_PTR sub_class,
	::DWORD_PTR ref_data)
{
	switch (msg)
	{
		case WM_MOUSEHOVER:
			SendMessage(wnd, WM_SETCURSOR, NULL, NULL);
			break;

		case WM_SETCURSOR:
		{
			static ::HCURSOR cursor{ ::LoadCursorW(nullptr, IDC_HAND) };
			::SetCursor(cursor);
			break;
		}

		default:
			return ::DefSubclassProc(wnd, msg, wparam, lparam);
	}

	return 0;
}