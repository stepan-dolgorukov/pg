#include "Main.h"
#include "MainWindow.h"

#include "WindowsFeatures.h"
#include "AboutWindow.h"
#include "InfoButton.h"

#include "ControlButtons.h"
#include "GenerationButton.h"

#include <string>
#include <cstring>
#include <cstdlib>

::LRESULT __stdcall main_wnd_proc(
	::HWND window,
	::UINT msg,
	::WPARAM w_param,
	::LPARAM l_param);

void handle_error(const ::DWORD id);

int __stdcall wWinMain(
	::HINSTANCE inst,
	::HINSTANCE prev_inst,
	::LPWSTR cmd_line,
	int cmd_show)
{
	const std::wstring
		main_wnd_class_name{ L"pg" },
		main_wnd_caption{ L"pg by DolgorukovGTA" };

	// текущий дескриптор приложения
	inst = ::GetModuleHandleW(nullptr);

	::HANDLE inst_mutex{ ::CreateMutexW(
		nullptr,
		TRUE,
		main_wnd_caption.c_str()) };

	// проверка ниже позволит избежать запуска 
	// нескольких экземпляров приложения
	if (nullptr == inst_mutex ||
		ERROR_ALREADY_EXISTS == ::GetLastError())
	{
		const ::HWND wnd{ FindWindowW(
			main_wnd_class_name.c_str(),
			main_wnd_caption.c_str()) };

		if (nullptr != wnd) 
		{
			::SetForegroundWindow(wnd);
		}

		::CloseHandle(inst_mutex);
		return EXIT_SUCCESS;
	}

	const ATOM reg_res{ ::registerWindowClass(
		main_wnd_class_name.c_str(),
		main_wnd_proc,
		inst) };

	if (!reg_res)
	{
		::handle_error(::GetLastError());
	}

	// размеры окна
	const std::uint16_t
		main_wnd_w{ 250u }, // ширина
		main_wnd_h{ 315u }; // высота

	const auto pos_pair{ ::getWindowCenterCoordinates(
		main_wnd_w,
		main_wnd_h) };

	const HWND main_wnd{ ::CreateWindowExW(
		WS_EX_LAYERED,
		main_wnd_class_name.c_str(),
		main_wnd_caption.c_str(),
		WS_POPUP,
		pos_pair.first,
		pos_pair.second,
		main_wnd_w,
		main_wnd_h,
		nullptr,
		nullptr,
		inst,
		nullptr) };


	if (nullptr == main_wnd)
	{
		::handle_error(::GetLastError());
	}

	// анимация появления окна в сообщении WM_SIZE
	// на текущий момент окно не видно
	::ShowWindow(main_wnd, SW_SHOWNORMAL);
	::UpdateWindow(main_wnd);

	MSG msg{};

	while (::GetMessageW(&msg, nullptr, 0u, 0u))
	{
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	}

	return static_cast<int>(msg.wParam);
}

::LRESULT __stdcall ::main_wnd_proc(
	::HWND wnd,
	::UINT msg,
	::WPARAM w_param,
	::LPARAM l_param)
{
	static bool check_box_vals[5u] = { true, true, true, true, true };
	static ::HWND 
		pass_amnt_form{},
		pass_len_form{};

	// объявляем переменные под шрифты, которые будут задействованы в сообщении WM_PAINT
	// шрифты инициализируются в сообщении WM_CREATE
	static ::HFONT
		caption_font{},
		text_font{};

	// !!!
	// название класса и заголовок окна "О программе". Вынесено в эту область ...
	// ... чтобы не приходилось корректировать проверку на существование (FindWindow) ...
	// окна при дальнейшем изменении названия класса или заголовка.
	static ::LPCWSTR
		info_wnd_class_name{ L"About pg" },
		info_wnd_caption{ L"About pg" };

	static const ::HINSTANCE
		inst{ GetModuleHandleW(nullptr) };

	switch (msg)
	{
		// плавная анимация появления
		case WM_SIZE:
		{
			if (SIZE_RESTORED == w_param)
			{
				std::thread thr([wnd] {
					::smoothWindowApprearance(wnd);
				});

				thr.detach();
			}

			break;
		}

		// Устанавливаем соответствующий курсор, когда пользователь ...
		// ... перетаскивает окно
		case WM_MOVING:
		{
			static const ::HCURSOR cursor{
				::LoadCursorW(nullptr, IDC_SIZEALL) };
			
			::SetCursor(cursor);
			break;
		}

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
				18,
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

			// кнопка "Закрыть" (красного цвета):
			const ::HWND close_btn{ ::CreateWindowExW(
				0u,
				L"Button",
				NULL,
				WS_CHILD | WS_VISIBLE | BS_BITMAP,
				220,
				5,
				25,
				25,
				wnd,
				reinterpret_cast<::HMENU>(::ControlsID::CLOSE_BUTTON),
				inst,
				nullptr) };

			// подгружаем иконку кнопки из ресурсов:
			const ::HBITMAP close_btn_bmp{ ::LoadBitmapW(
				inst,
				MAKEINTRESOURCEW(IDB_CLOSE_BUTTON)) };

			// устанавливаем иконку кнопке:
			::SendMessageW(
				close_btn,
				BM_SETIMAGE,
				IMAGE_BITMAP,
				reinterpret_cast<::LPARAM>(close_btn_bmp));

			// Выгружаем битмап из памяти:
			::DeleteObject(close_btn_bmp);

			// "Кнопочная" процедура для отработки необходимых сообщений:
			::SetWindowSubclass(
				close_btn,
				controlButtonProcedure,
				0u,
				0u);

			// Кнопка "Свернуть" (жёлтого цвета):
			const ::HWND minimize_btn{ ::CreateWindowExW(
				0u,
				L"Button",
				nullptr,
				WS_CHILD | WS_VISIBLE | BS_BITMAP,
				190,
				5,
				25,
				25, 
				wnd,
				reinterpret_cast<::HMENU>(::ControlsID::MINIMIZE_BUTTON),
				inst,
				nullptr) };

			// Подгружаем битмап для кнопки из ресурсов:
			const ::HBITMAP minimize_btn_bmp{ LoadBitmapW(
				inst,
				MAKEINTRESOURCEW(IDB_MINIMIZE_BUTTON)) };

			// Устанавливаем загруженный битмап кнопке:
			::SendMessageW(
				minimize_btn,
				BM_SETIMAGE,
				IMAGE_BITMAP,
				reinterpret_cast<::LPARAM>(minimize_btn_bmp));

			// Выгружаем битмап из памяти:
			::DeleteObject(minimize_btn_bmp);

			// Аналогично с кнопкой "Свернуть":
			::SetWindowSubclass(
				minimize_btn,
				controlButtonProcedure,
				0u,
				0u);

			// Поле ввода "Количество паролей":
			pass_amnt_form = ::CreateWindowExW(
				0u,
				L"Edit",
				L"64",
				WS_CHILD | WS_VISIBLE | ES_NUMBER,
				195,
				49,
				40,
				20,
				wnd,
				nullptr,
				inst,
				nullptr);

			::SendMessageW(
				pass_amnt_form,
				WM_SETFONT,
				reinterpret_cast<::WPARAM>(text_font),
				0);

			// Поле ввода "Длина каждого пароля":
			pass_len_form = ::CreateWindowExW(
				0u,
				L"Edit",
				L"32",
				WS_CHILD | WS_VISIBLE | ES_NUMBER,
				195,
				72,
				40,
				20,
				wnd,
				nullptr,
				inst,
				nullptr);

			::SendMessageW(
				pass_len_form,
				WM_SETFONT,
				reinterpret_cast<::WPARAM>(text_font),
				0);

			// Создание чекбоксов:
			auto h_offset{ 100u };
			for (auto i{ 3u }; i <= 7u; i++)
			{
				::CreateWindowExW(
					0u,
					L"Button",
					nullptr,
					WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
					10,
					h_offset,
					15,
					25, wnd,
					reinterpret_cast<::HMENU>(i),
					inst,
					nullptr);

				CheckDlgButton(wnd, i, BST_CHECKED);
				h_offset += 22u;
			}

			// Кнопка "Сгенерировать пароли":
			const ::HWND generation_btn{ CreateWindowExW(
				0u,
				L"Button",
				nullptr,
				WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
				10,
				220,
				230,
				40,
				wnd,
				reinterpret_cast<::HMENU>(ControlsID::GENERATE_BUTTON),
				inst,
				nullptr) };

			::SetWindowSubclass(
				generation_btn,
				generationButtonProcedure,
				0u,
				0u);

			// Кнопка "О программе":
			const ::HWND info_btn{ ::CreateWindowExW(
				0u,
				L"Button",
				nullptr,
				WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
				10,
				265,
				230,
				40,
				wnd,
				reinterpret_cast<::HMENU>(::ControlsID::INFO_BUTTON),
				inst,
				nullptr) };

			::SetWindowSubclass(
				info_btn,
				infoButtonProcedure,
				0u,
				0u);

			break;
		}

		case WM_COMMAND:
		{
			// Младшее слово определяет идентификатор пункта меню
			switch (LOWORD(w_param))
			{
			case ::ControlsID::CLOSE_BUTTON:
			{
				const ::HWND info_wnd{ ::FindWindowW(
					info_wnd_class_name,
					info_wnd_caption) };

				if (nullptr != info_wnd)
				{
					std::thread thr(
						smoothWindowHide,
						info_wnd,
						true);
					thr.detach();
				}

				::smoothWindowHide(wnd, true);

				// Выгружаем шрифты:
				::DeleteObject(caption_font);
				::DeleteObject(text_font);
				
				::ExitProcess(EXIT_SUCCESS);
			}

			case ::ControlsID::MINIMIZE_BUTTON:
			{
				const ::HWND info_wnd{ ::FindWindowW(
					info_wnd_class_name,
					info_wnd_caption) };

				if (nullptr != info_wnd)
				{
					std::thread thr(smoothWindowHide, info_wnd, true);
					thr.detach();
				}

				::smoothWindowHide(wnd, false);
				::ShowWindow(wnd, SW_MINIMIZE);
				break;
			}

			case ::ControlsID::GENERATE_BUTTON:
			{
				// Проверим значения чекбоксов кроме ЧБ "Избегать повторений"
				auto true_values_amnt{ 0u };

				for (auto i{ 0u }; i < 4u; i++)
				{
					if (check_box_vals[i])
					{
						++true_values_amnt;
					}
				}

				if (0u == true_values_amnt)
				{
					::MessageBoxW(
						wnd,
						L"Галочка должна стоять как минимум у одного из чекбоксов. \
						Чекбокс \"Избегать повторений\" не считается.",
						L"Ошибка:", MB_ICONERROR);
					break;
				}

				// Считываем информацию с полей "Количество паролей" и ...
				// ... "Длина каждого пароля":

				char
					input_pass_amnt[16u]{ 0 },
					input_pass_len[16u]{ 0 };

				::GetWindowTextA(
					pass_amnt_form,
					input_pass_amnt,
					sizeof(input_pass_amnt) / sizeof(char) - 1);

				::GetWindowTextA(
					pass_len_form,
					input_pass_len,
					sizeof(input_pass_len) / sizeof(char) - 1);

				auto
					pass_amnt{ 0u },
					pass_len{ 0u };

				pass_amnt = std::atoi(input_pass_amnt);
				pass_len = std::atoi(input_pass_len);

				if (0u == pass_amnt &&
					0u == pass_len)
				{
					MessageBox(wnd,
						L"Поля «Кол-во паролей» и «Длина каждого пароля». \
						 Введённые значения не соответствуют ограничениям.\n\
						 Минимальное кол-во паролей: 5, максимальное: 4096.\n\
						 Минимальная длина пароля: 4, максимальная: 256.",
						L"Ошибка ввода:", MB_ICONERROR);
					break;
				}

				else
				{
					if (0u == pass_amnt ||
						4096u < pass_amnt)
					{
						::MessageBoxW(
							wnd,
							L"Введеное значение не соответствует ограничениям.\n\
							 Минимальное кол-во паролей: 1, максимальное: 4096.",
							L"Ошибка ввода:",
							MB_ICONERROR);

						break;
					}

					if (0u == pass_len ||
						4u > pass_len ||
						256 < pass_len)
					{
						::MessageBoxW(
							wnd,
							L"Введеное значение не соответствует ограничениям\n\
							 Минимальная длина пароля: 4, максимальная: 256.",
							L"Ошибка ввода:",
							MB_ICONERROR);

						break;
					}
				}

				::SYSTEMTIME st{};
				::OPENFILENAME ofn{};
				wchar_t fileName[MAX_PATH]{};

				::GetLocalTime(&st);
				::wsprintfW(
					fileName,
					L"(P = %u, L = %u) %u.%u.%u %u-%u-%u-%u.txt",
					pass_amnt,
					pass_len,
					st.wMonth,
					st.wDay,
					st.wYear,
					st.wHour,
					st.wMinute,
					st.wSecond,
					st.wMilliseconds);

				ofn.lStructSize = sizeof(OPENFILENAME);
				ofn.hwndOwner = wnd;
				ofn.hInstance = nullptr;
				// Строка ниже действительно должна заканчиваться 2-умя нуль-терминаторами:
				ofn.lpstrFilter = L"Текстовый файл (*.txt)\0.txt\0";
				ofn.lpstrCustomFilter = nullptr;
				ofn.nMaxCustFilter = 256;
				ofn.nFilterIndex = 0;
				ofn.lpstrFile = fileName;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFileTitle = nullptr;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = nullptr;
				ofn.lpstrTitle = nullptr;
				ofn.Flags = 0u;
				ofn.nFileOffset = 0u;
				ofn.nFileExtension = 0u;
				ofn.lpstrDefExt = nullptr;
				ofn.lCustData = 0;
				ofn.lpfnHook = nullptr;
				ofn.lpTemplateName = nullptr;

				if (0u == ::GetSaveFileNameW(&ofn))
				{
					// !!!
					break;
				}

				auto start_time{ ::GetTickCount64() };

				// Функция ниже возвращает динамический массив с сгенерированными паролями, время их генерации
				// Указатель на указатель, т.к массив состоит из Си-строк
				const auto passwords{ ::generatePasswords(
					pass_amnt,
					pass_len,
					check_box_vals) };

				// засекаем время генерации
				auto generation_time{ GetTickCount64() - start_time };

				// обновляем счётчик, будем считать время записи
				start_time = GetTickCount64();

				const ::HANDLE output_file{ ::CreateFileW(
					ofn.lpstrFile,
					FILE_WRITE_DATA,
					0u,
					nullptr,
					OPEN_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					nullptr) };

				if (INVALID_HANDLE_VALUE == output_file)
				{
					::handle_error(::GetLastError());
					break;
				}

				// кол-во паролей максимально может достигать четырёхзначного числа
				char tmp_buf[5u]{};
				auto write_buf_size{ 0u };

				for (uint16_t i = 0; i < pass_amnt; i++)
				{
					sprintf(tmp_buf, "%i", i + 1);

					// y + 1 + 1 + x + 1 + 1
					write_buf_size = std::strlen(tmp_buf) + 
									  1u +
									  1u + 
									  std::strlen(passwords[i]) + 1;

					// В последней строке не нужно писать \n, поэтому строка становится на 1 символ меньше
					if (i != pass_amnt - 1) // Учитываем сообщение выше
					{
						++write_buf_size;
					}

					char* write_buf{ new char[write_buf_size] };

					std::sprintf(write_buf,
								(i != pass_amnt - 1) ? "%i: %s\n" : "%i: %s",
								 i + 1, passwords[i]);

					::BOOL res{ ::WriteFile(
						output_file,
						write_buf,
						write_buf_size - 1,
						0u,
						nullptr) };

					if (0u == res)
					{
						wchar_t szMsg[256u]{0};

						::wsprintfW(szMsg,
							L"Не удалось записать строку №%i в файл.\n\
							 Если Вы нажмёте «Повторить попытку» (Retry), то приложение попытается вновь записать в файл эту строку.\n\
							 Если нажмёте «Отмена» (Cancel), то приложение остановит запись в файл.", i + 1);

						const auto msg_box_res{::MessageBoxW(
							wnd,
							szMsg,
							L"Уведомление:",
							MB_RETRYCANCEL | MB_ICONERROR | MB_APPLMODAL) };
						
						if (IDRETRY == msg_box_res)
						{
							--i;
						}

						else
						{
							delete[] write_buf;
							CloseHandle(output_file);
							return 0;
						}

					}

					delete[] write_buf;
				}

				CloseHandle(output_file);

				// засекаем время записи в файл
				const auto write_time{ GetTickCount64() - start_time };

				// Массивы, в которые будут писаться время записи и генерации:
				wchar_t generation_time_buf[10]{};
				wchar_t write_time_buf[10]{};
				wchar_t notification[320]{};

				(0u != generation_time) ? 
					::wsprintfW(generation_time_buf, L"%u", generation_time) : 
					::wsprintfW(generation_time_buf, L"%s\n", L"< 1");

				(0u != write_time) ?
					::wsprintfW(write_time_buf, L"%u", write_time) :
					::wsprintfW(write_time_buf, L"%s\n", L"< 1");

				::wsprintfW(notification,
					L"Пароли записаны в файл \"%s\"\nВремя генерации: %s мс\nВремя записи в файл: %s мс",
					ofn.lpstrFile,
					generation_time_buf,
					write_time_buf);

				::MessageBoxW(wnd, notification, L"Уведомление:", MB_OK);

				// Освобождение выделенной памяти под массив с паролями:
				for (auto i{0u}; i < pass_amnt; i++)
					delete[] passwords[i];
				delete[] passwords;

				break;
			}

			case ::ControlsID::INFO_BUTTON:
			{
				::HWND info_wnd{ ::FindWindowW(
					info_wnd_class_name,
					info_wnd_caption) };

				if (nullptr != info_wnd)
				{
					::SetForegroundWindow(info_wnd);
					break;
				}

				// Процедура окна "О программе" находится в файле AboutWindow.cpp
				static ::ATOM reg_res{ ::registerWindowClass(
					info_wnd_class_name,
					aboutWindowProcedure, inst) };

				if (0u == reg_res)
				{
					::handle_error(GetLastError());
				}

				static const uint32_t
				info_wnd_w{ 250u },
				info_wnd_h{ info_wnd_w };

				const
				std::pair<uint32_t,
					      uint32_t> pos_pair{
					::getWindowCenterCoordinates(
						info_wnd_w,
						info_wnd_h) };

				// Окно "О программе"
				info_wnd = CreateWindowExW(
					WS_EX_LAYERED,
					info_wnd_class_name,
					info_wnd_caption,
					WS_POPUP,
					pos_pair.first,
					pos_pair.second,
					info_wnd_w,
					info_wnd_h,
					nullptr,
					nullptr,
					inst,
					nullptr);

				if (nullptr != info_wnd)
				{
					::handle_error(GetLastError());
				}

				::UpdateWindow(info_wnd);
				::ShowWindow(info_wnd, SW_SHOWNORMAL);

				std::thread thr([info_wnd] {
					smoothWindowApprearance(info_wnd);
				});

				thr.detach();
				break;
			}
			}

			// Обработка чекбоксов:
			if (LOWORD(w_param) >= 3 && LOWORD(w_param) <= 7) // [3; 7] - чекбоксы
			{
				BOOL result = IsDlgButtonChecked(wnd, LOWORD(w_param)); // получаем текущее состояние
				CheckDlgButton(wnd, LOWORD(w_param), result ? BST_UNCHECKED : BST_CHECKED); // меняем визуальное состояние
				check_box_vals[LOWORD(w_param) - 3] = result ? false : true; // присваиваем соответствующее значение массиву логических значений чекбоксов
			}
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps{};
			HDC hdc = BeginPaint(wnd, &ps);

			SelectObject(hdc, caption_font);
			SetBkColor(hdc, RGB(29, 29, 29));
			SetTextColor(hdc, RGB(255, 255, 255));

			// Отображение текста:
			TextOut(hdc, 10, 6, L"Генератор паролей", strlen("Генератор паролей"));

			SelectObject(hdc, text_font);

			TextOut(hdc, 10, 52, L"Кол-во паролей:", lstrlen(L"Кол-во паролей:"));
			TextOut(hdc, 10, 74, L"Длина каждого пароля:", lstrlen(L"Длина каждого пароля:"));

			TextOut(hdc, 30, 103, L"Использовать цифры", lstrlen(L"Использовать цифры"));
			TextOut(hdc, 30, 125, L"Использовать заглавные буквы", lstrlen(L"Использовать заглавные буквы"));
			TextOut(hdc, 30, 147, L"Использовать строчные буквы", lstrlen(L"Использовать строчные буквы"));
			TextOut(hdc, 30, 169, L"Использовать символы", lstrlen(L"Использовать символы"));
			TextOut(hdc, 30, 191, L"Избегать повторений", lstrlen(L"Избегать повторений"));

			EndPaint(wnd, &ps);
			break;
		}

		case WM_CTLCOLORSTATIC:
		{
			// Инициализируем этот описатель только 1 раз. Иначе будет утечка памяти:
			static HBRUSH hBrush = CreateSolidBrush(RGB(29, 29, 29));
			return reinterpret_cast<LRESULT>(hBrush);
		}

		// Трюк ниже позволяет перетаскивать окно за любую часть окна:
		case WM_LBUTTONDOWN: // Ловим нажатие левой кнопки мыши
			// Отправляемое сообщение буквально значит "Нажата ЛКМ на заголовке окна":
			SendMessage(wnd, WM_NCLBUTTONDOWN, HTCAPTION, NULL);
			break;

		default:
			return DefWindowProc(wnd, msg, w_param, l_param);
		}

	return 0;
}


void
::handle_error(const ::DWORD id)
{
	wchar_t szMsg[64]{};

	::wsprintfW(szMsg, L"Приложение завершает работу с " 
					   "ошибкой. ID ошибки: %u.", id);

	::MessageBoxW(nullptr, szMsg, L"Ошибка:", MB_ICONERROR);
	::ExitProcess(EXIT_FAILURE);
}
