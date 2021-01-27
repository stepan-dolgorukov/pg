#include "info_btn.h"

#include <Windows.h>
#include <CommCtrl.h> // DefSubclassProc

LRESULT __stdcall info_btn_proc(
	HWND info_btn,
	UINT msg,
	WPARAM wparam,
	LPARAM lparam,
	UINT_PTR sub_class,
	DWORD_PTR ref_data)
{
	UNREFERENCED_PARAMETER(sub_class);
	UNREFERENCED_PARAMETER(ref_data);

	static bool is_mouse_on_btn{ false };

	switch (msg)
	{
		case WM_PAINT: 
		{
			HFONT text_font{ ::CreateFontW(
				22,
				0,
				0,
				0,
				FW_NORMAL,
				FW_DONTCARE,
				0,
				0,
				DEFAULT_CHARSET,
				OUT_TT_ONLY_PRECIS,
				CLIP_CHARACTER_PRECIS,
				PROOF_QUALITY,
				FF_MODERN,
				L"Segoe UI")
			};

			::PAINTSTRUCT ps{};
			::HDC hdc{ BeginPaint(info_btn, &ps) };

			::SelectObject(hdc, text_font);
			::SetTextColor(hdc, RGB(255u, 255u, 255u));

			::COLORREF btn_bg_color{ (is_mouse_on_btn) ? RGB(45u, 45u, 45u) : RGB(35u, 35u, 35u) };
			::SetBkColor(hdc, btn_bg_color);
			::HBRUSH brush{ ::CreateSolidBrush(btn_bg_color) };

			::RECT crect{};
			GetClientRect(info_btn, &crect);
			FillRect(hdc, &crect, brush);

			::DrawTextW(
				hdc,
				L"О программе",
				lstrlenW(L"О программе"),
				&crect,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			::DeleteObject(text_font);
			::DeleteObject(brush);
			::EndPaint(info_btn, &ps);

			break;
		}

		case WM_MOUSEMOVE: 
		{
			::TRACKMOUSEEVENT tme{};
			tme.cbSize = sizeof(::TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER | TME_LEAVE;
			tme.dwHoverTime = 1,
			tme.hwndTrack = info_btn;

			TrackMouseEvent(&tme);
			break;
		}

		case WM_MOUSELEAVE: 
		{
			is_mouse_on_btn = false;
			::InvalidateRect(info_btn, nullptr, FALSE);
			break;
		}

		case WM_MOUSEHOVER: 
		{
			is_mouse_on_btn = true;
			::InvalidateRect(info_btn, nullptr, FALSE);
			::SendMessageW(info_btn, WM_SETCURSOR, 0, 0);

			break;
		}

		case WM_SETCURSOR:
			::SetCursor(LoadCursorW(nullptr, IDC_HAND));
			break;

		default:
			return ::DefSubclassProc(info_btn, msg, wparam, lparam);
		}

	return 0;
}