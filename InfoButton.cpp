#include "Main.h"

LRESULT CALLBACK infoButtonProcedure(HWND hInfoButWindow, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	UNREFERENCED_PARAMETER(uIdSubclass);
	UNREFERENCED_PARAMETER(dwRefData);

	static bool isMouseOnButton = false;

	switch (uMsg)
	{

		case WM_PAINT: 
		{
			HFONT hTextFont = CreateFont(22, 0, 0, 0, FW_NORMAL, FW_DONTCARE, 0, 0,
				DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_CHARACTER_PRECIS,
				PROOF_QUALITY, FF_MODERN, L"Segoe UI");

			PAINTSTRUCT ps{};
			HDC hdc = BeginPaint(hInfoButWindow, &ps);

			SelectObject(hdc, hTextFont);
			SetTextColor(hdc, RGB(255, 255, 255));

			COLORREF buttonBackgroundColor = (isMouseOnButton) ? RGB(45, 45, 45) : RGB(35, 35, 35);
			SetBkColor(hdc, buttonBackgroundColor);
			HBRUSH backgroundBrush = CreateSolidBrush(buttonBackgroundColor);

			RECT crect{};
			GetClientRect(hInfoButWindow, &crect);
			FillRect(hdc, &crect, backgroundBrush);

			DrawText(hdc, L"О программе", lstrlen(L"О программе"),
				&crect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			DeleteObject(hTextFont);
			DeleteObject(backgroundBrush);

			EndPaint(hInfoButWindow, &ps);
			break;
		}

		case WM_MOUSEMOVE: 
		{
			TRACKMOUSEEVENT tme{};
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER | TME_LEAVE;
			tme.dwHoverTime = 1,
			tme.hwndTrack = hInfoButWindow;

			TrackMouseEvent(&tme);
			break;
		}

		case WM_MOUSELEAVE: 
		{
			isMouseOnButton = false;
			InvalidateRect(hInfoButWindow, NULL, FALSE);
			break;
		}

		case WM_MOUSEHOVER: 
		{
			isMouseOnButton = true;
			InvalidateRect(hInfoButWindow, NULL, FALSE);

			SendMessage(hInfoButWindow, WM_SETCURSOR, NULL, NULL);
			break;
		}

		case WM_SETCURSOR:
			SetCursor(LoadCursor(NULL, IDC_HAND));
			break;

		default:
			return DefSubclassProc(hInfoButWindow, uMsg, wParam, lParam);
		}

	return 0;
}