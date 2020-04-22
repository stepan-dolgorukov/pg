#include "Main.h"


LRESULT CALLBACK controlButtonProcedure(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	UNREFERENCED_PARAMETER(uIdSubclass);
	UNREFERENCED_PARAMETER(dwRefData);

	switch (uMsg)
	{

	case WM_MOUSEHOVER: {
		SendMessage(hWindow, WM_SETCURSOR, NULL, NULL);
		break;
	}

	case WM_SETCURSOR: {
		SetCursor(LoadCursor(NULL, IDC_HAND));
		break;
	}

	default:
		return DefSubclassProc(hWindow, uMsg, wParam, lParam);
	}

	return 0;
}