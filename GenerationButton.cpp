#include "Main.h"

LRESULT CALLBACK generationButtonProcedure(HWND hGenButWindow, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
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
			HDC hdc = BeginPaint(hGenButWindow, &ps);

			SelectObject(hdc, hTextFont);
			SetTextColor(hdc, RGB(255, 255, 255));

			COLORREF buttonBackgroundColor = (isMouseOnButton) ? RGB(45, 45, 45) : RGB(35, 35, 35);
			SetBkColor(hdc, buttonBackgroundColor);
			HBRUSH backgroundBrush = CreateSolidBrush(buttonBackgroundColor);

			RECT crect{};
			GetClientRect(hGenButWindow, &crect);
			FillRect(hdc, &crect, backgroundBrush);

			DrawText(hdc, L"Сгенерировать пароли", lstrlen(L"Сгенерировать пароли"),
				&crect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			DeleteObject(hTextFont);
			DeleteObject(backgroundBrush);

			EndPaint(hGenButWindow, &ps);
			break;
		}

		case WM_MOUSEMOVE: 
		{
			TRACKMOUSEEVENT tme{};
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER | TME_LEAVE;
			tme.dwHoverTime = 1;
			tme.hwndTrack = hGenButWindow;

			TrackMouseEvent(&tme);
			break;
		}

		case WM_MOUSELEAVE:
			isMouseOnButton = false;
			InvalidateRect(hGenButWindow, NULL, FALSE); // перерисовка окна
			break;

		case WM_MOUSEHOVER:
			isMouseOnButton = true;
			InvalidateRect(hGenButWindow, NULL, FALSE);
			SendMessage(hGenButWindow, WM_SETCURSOR, NULL, NULL);
			break;


		case WM_SETCURSOR:
		{
			static HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
			SetCursor(hCursor);
			break;
		}

		default:
			return DefSubclassProc(hGenButWindow, uMsg, wParam, lParam);
	}

	return 0;
}