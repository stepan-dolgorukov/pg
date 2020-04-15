#include "Main.h"
#include "WindowsFeatures.h"
#include "ControlButtons.h" // кнопки "Закрыть", "Свернуть"
#include "Version.h"

LRESULT CALLBACK aboutWindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP aboutProgramBitmap;
	static HFONT captionFont, textFont;

	switch (message)
	{

	case WM_CREATE: {

		captionFont = CreateFont(22, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
			DEFAULT_QUALITY, FF_SWISS, L"Segoe UI");

		textFont = CreateFont(18, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
			DEFAULT_QUALITY, FF_SWISS, L"Segoe UI");

		aboutProgramBitmap = LoadBitmap(GetModuleHandle(NULL),
			MAKEINTRESOURCE(IDB_ABOUT_ICON));

		// Кнопка "Закрыть":
		HWND closeButton = CreateWindow(TEXT("Button"), NULL,
			WS_CHILD | WS_VISIBLE | BS_BITMAP, 220, 5, 25, 25,
			window, reinterpret_cast<HMENU>(CLOSE_BUTTON), NULL, NULL);

		// Загружаем картинку из ресурсов:
		HBITMAP closeButtonBitmap = LoadBitmap(GetModuleHandle(NULL), 
			MAKEINTRESOURCE(IDB_CLOSE_BUTTON));

		// Устанавливаем картинку кнопке:
		SendMessage(closeButton, BM_SETIMAGE, IMAGE_BITMAP, reinterpret_cast<LPARAM>(closeButtonBitmap));
		DeleteObject(closeButtonBitmap); // картинка больше нам не понадобится, выгружаем её

		// Устанавливаем отдельную процедуру для кнопки, чтобы изменять курсор при ...
		// ... наведении на кнопку:
		SetWindowSubclass(closeButton, controlButtonProcedure, 0, 0);

		break;
	}

	case WM_PAINT: {

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(window, &ps);

		SetBkColor(hdc, RGB(29, 29, 29));
		SetTextColor(hdc, RGB(255, 255, 255));
		SelectObject(hdc, captionFont);

		TextOut(hdc, 10, 6, TEXT("Информация о программе"), strlen("Информация о программе"));

		HDC bitmapHdc = CreateCompatibleDC(NULL);
		SelectObject(bitmapHdc, aboutProgramBitmap);

		RECT rect;
		GetWindowRect(window, &rect);

		// Рассчёты производятся относительно размеров окна, в котором будет находиться изображение
		// Высота - 250 пикс., ширина - 250 пикс.
		uint8_t picX = static_cast<uint8_t>((rect.right - rect.left - 128) / 2);
		uint8_t picY = static_cast<uint8_t>(((rect.top - rect.bottom - 128) - 40) / 2);

		BitBlt(hdc, picX, picY, 128, 128, bitmapHdc, 0, 0, SRCCOPY);
		SelectObject(hdc, textFont);

		// Получаем координаты левого верхнего и правого нижнего углов клиентской части угла
		GetClientRect(window, &rect);

		// Изменяем эти значения, чтобы уменьшить этот прямоугольник, вследствие чего текст будет находиться ниже
		rect.top = 195;

		// Отображаем версию и дату релиза приложения:
		wchar_t infoText[64];
		wsprintf(infoText, L"Версия %s, релиз от %s\r\nАвтор: Dolgorukov", version, releaseDate);

		// Смысла отображать нуль-терминатор нет, поэтому используем strlen:
		DrawText(hdc, infoText, sizeof(infoText) / sizeof(wchar_t), &rect, DT_CENTER);

		DeleteDC(bitmapHdc);

		EndPaint(window, &ps);
		break;
	}

	case WM_COMMAND: {

		if (LOWORD(wParam) == CLOSE_BUTTON) {

			// Выгрузка большой иконки приложения:
			DeleteObject(aboutProgramBitmap);

			// Выгрузка шрифтов:
			DeleteObject(captionFont);
			DeleteObject(textFont);

			smoothWindowHide(window, true);
			break;
		}
	}

	case WM_LBUTTONDOWN:
		SendMessage(window, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;

	default:
		return DefWindowProc(window, message, wParam, lParam);

	}

	return 0;
}