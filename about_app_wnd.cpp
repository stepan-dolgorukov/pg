#include "resource.h"
#include "constants.h"
#include "control_btn.h"
#include "version.h"
#include "wnd_util.h"

#include <Windows.h>
#include <CommCtrl.h> // SetWindowSubclass

::LRESULT __stdcall about_app_wnd_proc(
	::HWND wnd,
	::UINT msg,
	::WPARAM wparam,
	::LPARAM lparam)
{
	static ::HBITMAP app_large_bmp{nullptr};

	static ::HFONT 
		caption_font{nullptr},
		text_font{nullptr};

	switch (msg)
	{
		case WM_CREATE: 
		{
			caption_font = ::CreateFontW(
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
				L"Segoe UI");

			text_font = ::CreateFontW(
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
				L"Segoe UI");

			app_large_bmp = ::LoadBitmapW(
				::GetModuleHandleW(nullptr),
				MAKEINTRESOURCEW(IDB_ABOUT_ICON));

			// Кнопка "Закрыть":
			const ::HWND close_btn = ::CreateWindowExW(
				0u,
				L"Button",
				nullptr,
				WS_CHILD | WS_VISIBLE | BS_BITMAP,
				220,
				5,
				25,
				25,
				wnd, reinterpret_cast<::HMENU>(::CONTROLS_IDS::CLOSE_BUTTON),
				nullptr,
				nullptr);

			// Загружаем картинку из ресурсов:
			const ::HBITMAP close_btn_bmp{ ::LoadBitmapW(
				GetModuleHandleW(nullptr),
				MAKEINTRESOURCEW(IDB_CLOSE_BUTTON))
			};

			// Устанавливаем картинку кнопке:
			::SendMessageW(
				close_btn,
				BM_SETIMAGE,
				IMAGE_BITMAP,
				reinterpret_cast<::LPARAM>(close_btn_bmp));

			// Выгрузка установленного битмапа из памяти
			::DeleteObject(close_btn_bmp);

			// Устанавливаем отдельную процедуру для кнопки,
			// ... чтобы изменять курсор при наведении на кнопку. 
			::SetWindowSubclass(close_btn, ::control_btn_proc, 0, 0);

			break;
		}

		case WM_PAINT: 
		{
			::PAINTSTRUCT ps{};
			::HDC hdc{ ::BeginPaint(wnd, &ps) };
			::HDC bitmapHdc{ CreateCompatibleDC(nullptr) };
			::RECT crect{};

			::SetBkColor(hdc, RGB(29u, 29u, 29u));
			::SetTextColor(hdc, RGB(255u, 255u, 255u));
			::SelectObject(hdc, caption_font);

			::TextOutW(hdc, 10, 6, L"Информация о программе", ::lstrlenW(L"Информация о программе"));
			::SelectObject(bitmapHdc, app_large_bmp);

			::GetClientRect(wnd, &crect);

			// Рассчёты производятся относительно размеров окна, в котором будет находиться изображение
			// Высота - 250 пикс., ширина - 250 пикс.
			uint16_t picX = static_cast<uint16_t>((crect.right - crect.left - 128) / 2);
			uint16_t picY = static_cast<uint16_t>((crect.bottom - crect.top - 128 - 30) / 2); 
			// приподнимаю от центра на 30 пикселей, чтобы смотрелось красиво в совокупности с текстом

			::BitBlt(hdc, picX, picY, 128, 128, bitmapHdc, 0, 0, SRCCOPY);
			::SelectObject(hdc, text_font);

			// Получаем координаты левого верхнего и правого нижнего углов клиентской части угла
			::GetClientRect(wnd, &crect);

			// Изменяем эти значения, чтобы уменьшить этот прямоугольник, вследствие чего текст будет находиться ниже
			crect.top = 195;

			// Отображаем версию и дату релиза приложения:
			wchar_t infoText[70]{};
			::wsprintfW(infoText, L"Версия %s, релиз от %s\r\nАвтор: DolgorukovGTA", version, releaseDate);

			uint8_t nullTerminatorPos = 0;
			while (infoText[nullTerminatorPos] != '\0') {
				++nullTerminatorPos;
			}

			DrawTextW(hdc, infoText, nullTerminatorPos, &crect, DT_CENTER);
			DeleteDC(bitmapHdc);
			EndPaint(wnd, &ps);

			break;
		}

		case WM_COMMAND:
		{
			if (::CONTROLS_IDS::CLOSE_BUTTON == wparam)
			{
				// Выгрузка большой иконки приложения:
				::DeleteObject(app_large_bmp);

				// Выгрузка шрифтов:
				::DeleteObject(caption_font);
				::DeleteObject(text_font);

				::smooth_wnd_hide(wnd, true);
			}

			break;
		}

		case WM_LBUTTONDOWN:
			SendMessageW(wnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
			break;

		case WM_MOVING:
			::SetCursor(::LoadCursorW(nullptr, IDC_SIZEALL));
			break;

		default:
			return ::DefWindowProcW(wnd, msg, wparam, lparam);
	}

	return 0;
}