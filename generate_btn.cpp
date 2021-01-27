#include "generate_btn.h"

#include <Windows.h>
#include <CommCtrl.h> // DefSubclassProc

::LRESULT __stdcall ::generate_btn_proc(
	::HWND gen_btn,
	::UINT msg,
	::WPARAM wparam,
	::LPARAM lparam,
	::UINT_PTR sub_class,
	::DWORD_PTR ref_data)
{
	UNREFERENCED_PARAMETER(sub_class);
	UNREFERENCED_PARAMETER(ref_data);

	static bool is_mouse_on_btn{ false };

	switch (msg) 
	{
		case WM_PAINT: 
		{
			const ::HFONT text_font{ ::CreateFontW(
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
			   L"Segoe UI") };

			::PAINTSTRUCT ps{};
			::HDC hdc{ BeginPaint(gen_btn, &ps) };

			::SelectObject(hdc, text_font);
			::SetTextColor(hdc, RGB(255u, 255u, 255u));

			::COLORREF background_color{ (is_mouse_on_btn) ? RGB(45, 45, 45) : RGB(35, 35, 35) };
			::SetBkColor(hdc, background_color);
			::HBRUSH backgroundBrush{ ::CreateSolidBrush(background_color) };

			::RECT crect{};
			::GetClientRect(gen_btn, &crect);
			::FillRect(hdc, &crect, backgroundBrush);

			::DrawTextW(
				hdc,
				L"Сгенерировать пароли",
				::lstrlenW(L"Сгенерировать пароли"),
				&crect,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			::DeleteObject(text_font);
			::DeleteObject(backgroundBrush);
			::EndPaint(gen_btn, &ps);

			break;
		}

		case WM_MOUSEMOVE: 
		{
			::TRACKMOUSEEVENT tme{};
			tme.cbSize = sizeof(::TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER | TME_LEAVE;
			tme.dwHoverTime = 1;
			tme.hwndTrack = gen_btn;

			::TrackMouseEvent(&tme);
			break;
		}

		case WM_MOUSELEAVE:
			is_mouse_on_btn = false;
			InvalidateRect(gen_btn, NULL, FALSE); // перерисовка окна
			break;

		case WM_MOUSEHOVER:
			is_mouse_on_btn = true;
			InvalidateRect(gen_btn, NULL, FALSE);
			SendMessage(gen_btn, WM_SETCURSOR, NULL, NULL);
			break;


		case WM_SETCURSOR:
		{
			static ::HCURSOR hCursor{ LoadCursorW(NULL, IDC_HAND) };
			SetCursor(hCursor);
			break;
		}

		default:
			return DefSubclassProc(gen_btn, msg, wparam, lparam);
	}

	return 0;
}