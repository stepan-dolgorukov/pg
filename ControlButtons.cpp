#include "Main.h"


LRESULT CALLBACK controlButtonProcedure(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	UNREFERENCED_PARAMETER(uIdSubclass);
	UNREFERENCED_PARAMETER(dwRefData);

	switch (msg)
	{

	case WM_MOUSEHOVER: {
		SendMessage(window, WM_SETCURSOR, 0, 0);
		break;
	}

	case WM_SETCURSOR: {
		SetCursor(LoadCursor(NULL, IDC_HAND));
		break;
	}

	default:
		return DefSubclassProc(window, msg, wParam, lParam);
	}

	return 0;
}