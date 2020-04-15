#include "Main.h"

LRESULT CALLBACK generationButtonProcedure(HWND genButWindow, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	UNREFERENCED_PARAMETER(uIdSubclass);
	UNREFERENCED_PARAMETER(dwRefData);

	static bool isMouseOnButton = false;

	switch (msg)
	{

	case WM_PAINT: {

		HFONT textFont = CreateFont(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
			DEFAULT_QUALITY, FF_SWISS, L"Segoe UI");

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(genButWindow, &ps);

		SelectObject(hdc, textFont);
		SetTextColor(hdc, RGB(255, 255, 255));

		COLORREF buttonBackgroundColor = (isMouseOnButton) ? RGB(45, 45, 45) : RGB(35, 35, 35);
		SetBkColor(hdc, buttonBackgroundColor);
		HBRUSH backgroundBrush = CreateSolidBrush(buttonBackgroundColor);

		RECT crect;
		GetClientRect(genButWindow, &crect);
		FillRect(hdc, &crect, backgroundBrush);

		DrawText(hdc, TEXT("Сгенерировать пароли"), strlen("Сгенерировать пароли"),
			&crect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		DeleteObject(textFont);
		DeleteObject(backgroundBrush);

		EndPaint(genButWindow, &ps);

		break;
	}

	case WM_MOUSEMOVE: {
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_HOVER | TME_LEAVE;
		tme.dwHoverTime = 1;
		tme.hwndTrack = genButWindow;

		TrackMouseEvent(&tme);
		break;
	}

	case WM_MOUSELEAVE: {
		isMouseOnButton = false;
		InvalidateRect(genButWindow, NULL, true); // перерисовка окна
		break;
	}

	case WM_MOUSEHOVER: {
		isMouseOnButton = true;
		InvalidateRect(genButWindow, NULL, true);

		SendMessage(genButWindow, WM_SETCURSOR, NULL, NULL);
		break;
	}


	case WM_SETCURSOR: {
		SetCursor(LoadCursor(NULL, IDC_HAND));
		break;
	}

	default:
		return DefSubclassProc(genButWindow, msg, wParam, lParam);
	}

	return 0;
}