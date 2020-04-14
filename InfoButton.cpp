#include "Main.h"

LRESULT CALLBACK infoButtonProcedure(HWND infoButWindow, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	UNREFERENCED_PARAMETER(uIdSubclass);
	UNREFERENCED_PARAMETER(dwRefData);

	static bool isMouseOnButton = false;

	switch (msg)
	{

	case WM_PAINT: {

		HFONT textFont = CreateFont(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
			DEFAULT_QUALITY, FF_SWISS, "Segoe UI");

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(infoButWindow, &ps);

		SelectObject(hdc, textFont);
		SetTextColor(hdc, RGB(255, 255, 255));

		COLORREF buttonBackgroundColor = (isMouseOnButton) ? RGB(45, 45, 45) : RGB(35, 35, 35);
		SetBkColor(hdc, buttonBackgroundColor);
		HBRUSH backgroundBrush = CreateSolidBrush(buttonBackgroundColor);

		RECT crect;
		GetClientRect(infoButWindow, &crect);
		FillRect(hdc, &crect, backgroundBrush);

		DrawText(hdc, "О программе", strlen("О программе"),
			&crect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		DeleteObject(textFont);
		DeleteObject(backgroundBrush);

		EndPaint(infoButWindow, &ps);

		break;
	}

	case WM_MOUSEMOVE: {
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_HOVER | TME_LEAVE;
		tme.dwHoverTime = 1;
		tme.hwndTrack = infoButWindow;

		TrackMouseEvent(&tme);
		break;
	}

	case WM_MOUSELEAVE: {
		isMouseOnButton = false;
		InvalidateRect(infoButWindow, NULL, true);
		break;
	}

	case WM_MOUSEHOVER: {
		isMouseOnButton = true;
		InvalidateRect(infoButWindow, NULL, true);

		SendMessage(infoButWindow, WM_SETCURSOR, 0, 0);
		break;
	}


	case WM_SETCURSOR: {
		SetCursor(LoadCursor(NULL, IDC_HAND));
		break;
	}

	default:
		return DefSubclassProc(infoButWindow, msg, wParam, lParam);
	}

	return 0;
}