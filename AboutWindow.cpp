#include "Main.h"
#include "WindowsFeatures.h"
#include "ControlButtons.h" // кнопки "Закрыть", "Свернуть"
#include "Version.h"

LRESULT CALLBACK aboutWindowProcedure(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP 
		aboutProgramBitmap;

	static HFONT 
		hCaptionFont, 
		hTextFont;

	switch (uMsg)
	{

		case WM_CREATE: 
		{
			hCaptionFont = CreateFont(22, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
				DEFAULT_QUALITY, FF_SWISS, L"Segoe UI");

			hTextFont = CreateFont(18, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
				DEFAULT_QUALITY, FF_SWISS, L"Segoe UI");

			aboutProgramBitmap = LoadBitmap(GetModuleHandle(NULL),
				MAKEINTRESOURCE(IDB_ABOUT_ICON));

			// Кнопка "Закрыть":
			HWND closeButton = CreateWindow(L"Button", NULL,
				WS_CHILD | WS_VISIBLE | BS_BITMAP, 220, 5, 25, 25,
				hWindow, reinterpret_cast<HMENU>(CLOSE_BUTTON), NULL, NULL);

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

		case WM_PAINT: 
		{
			PAINTSTRUCT ps{};
			HDC hdc = BeginPaint(hWindow, &ps);

			SetBkColor(hdc, RGB(29, 29, 29));
			SetTextColor(hdc, RGB(255, 255, 255));
			SelectObject(hdc, hCaptionFont);

			TextOut(hdc, 10, 6, L"Информация о программе", lstrlen(L"Информация о программе"));

			HDC bitmapHdc = CreateCompatibleDC(NULL);
			SelectObject(bitmapHdc, aboutProgramBitmap);

			RECT crect{};
			GetClientRect(hWindow, &crect);

			// Рассчёты производятся относительно размеров окна, в котором будет находиться изображение
			// Высота - 250 пикс., ширина - 250 пикс.
			uint16_t picX = static_cast<uint16_t>((crect.right - crect.left - 128) / 2);
			uint16_t picY = static_cast<uint16_t>((crect.bottom - crect.top - 128 - 30) / 2); 
			// приподнимаю от центра на 30 пикселей, чтобы смотрелось красиво в совокупности с текстом

			BitBlt(hdc, picX, picY, 128, 128, bitmapHdc, 0, 0, SRCCOPY);
			SelectObject(hdc, hTextFont);

			// Получаем координаты левого верхнего и правого нижнего углов клиентской части угла
			GetClientRect(hWindow, &crect);

			// Изменяем эти значения, чтобы уменьшить этот прямоугольник, вследствие чего текст будет находиться ниже
			crect.top = 195;

			// Отображаем версию и дату релиза приложения:
			wchar_t infoText[70]{};
			wsprintf(infoText, L"Версия %s, релиз от %s\r\nАвтор: DolgorukovGTA", version, releaseDate);

			uint8_t nullTerminatorPos = 0;
			while (infoText[nullTerminatorPos] != '\0') {
				++nullTerminatorPos;
			}

			DrawText(hdc, infoText, nullTerminatorPos, &crect, DT_CENTER);

			DeleteDC(bitmapHdc);

			EndPaint(hWindow, &ps);
			break;
		}

		case WM_COMMAND:
		{
			if (wParam == CLOSE_BUTTON)
			{

				// Выгрузка большой иконки приложения:
				DeleteObject(aboutProgramBitmap);

				// Выгрузка шрифтов:
				DeleteObject(hCaptionFont);
				DeleteObject(hTextFont);

				smoothWindowHide(hWindow, true);
			}

			break;
		}

		case WM_LBUTTONDOWN:
			SendMessage(hWindow, WM_NCLBUTTONDOWN, HTCAPTION, 0);
			break;

		case WM_MOVING:
			SetCursor(LoadCursor(NULL, IDC_SIZEALL));
			break;

		default:
			return DefWindowProc(hWindow, uMsg, wParam, lParam);
	}

	return 0;
}