// Необходимо для корректного отображения кастомных контролов:
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "comctl32.lib")

#include "constants.h" // controls ids
#include "about_app_wnd.h"
#include "info_btn.h"
#include "wnd_util.h"
#include "control_btn.h"
#include "resource.h"
#include "pass_gen.h"
#include "generate_btn.h"

#include <cstring>
#include <cstdlib>

#include <string>
#include <thread>

#include <Windows.h>
#include <CommCtrl.h> // SetWindowSubclass

::LRESULT __stdcall main_wnd_proc(
    const ::HWND wnd,
    const ::UINT msg,
    const ::WPARAM wparam,
    const ::LPARAM lparam);

void handle_error(const ::DWORD id);

int __stdcall wWinMain(
    const ::HINSTANCE inst,
    const ::HINSTANCE prev_inst,
    const ::LPWSTR cmd_line,
    const int cmd_show)
{
    // Чтобы избежать лишних для нас предупреждений, ...
    // ... пометим неиспользуемые параметры.
    UNREFERENCED_PARAMETER(prev_inst);
    UNREFERENCED_PARAMETER(cmd_line);
    UNREFERENCED_PARAMETER(cmd_show);

    const std::wstring
        main_wnd_class_name{ L"pg" },
        main_wnd_caption{ L"pg by DolgorukovGTA" };

    ::HANDLE inst_mutex{ ::CreateMutexW(
        nullptr,
        TRUE,
        main_wnd_caption.c_str())
    };

    // Проверка, позволяющая избегать запуска ...
    // ... нескольких экземпляров приложения:
    if (nullptr == inst_mutex || 
        ERROR_ALREADY_EXISTS == ::GetLastError())
    {
        const ::HWND wnd{ FindWindowW(
            main_wnd_class_name.c_str(),
            main_wnd_caption.c_str())
        };

        if (nullptr != wnd) 
        {
            ::SetForegroundWindow(wnd);
        }

        ::CloseHandle(inst_mutex);
        return EXIT_SUCCESS;
    }

    // Регистрация класса основного окна:
    const ATOM reg_res{ ::reg_wnd_class(
        main_wnd_class_name.c_str(),
        main_wnd_proc,
        inst)
    };

    // Если не удалось зарегистрировать класс:
    if (0u == reg_res)
    {
        ::handle_error(::GetLastError());
    }

    // Задаем размеры окна:
    const std::uint16_t
        main_wnd_w{ 250u }, // ширина;
        main_wnd_h{ 315u }; // высота.

    const auto pos_pair{ ::get_wnd_center(
        main_wnd_w,
        main_wnd_h)
    };

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
        nullptr)
    };

    // Неудачная инициализация главного окна.
    if (nullptr == main_wnd)
    {
        ::handle_error(::GetLastError());
    }

    ::ShowWindow(main_wnd, SW_SHOWNORMAL);
    ::UpdateWindow(main_wnd);

    // Цикл оконных сообщений.
    MSG msg{};

    while (::GetMessageW(&msg, nullptr, 0u, 0u))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}

::LRESULT __stdcall ::main_wnd_proc(
    const ::HWND wnd,
    const ::UINT msg,
    const ::WPARAM w_param,
    const ::LPARAM l_param)
{
    // По умолчанию все чекбоксы приведены в состояние "ВКЛ".
    static bool check_box_vals[5u] = {1u, 1u, 1u, 1u, 1u};

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
        // Плавная анимация появления.
        case WM_SIZE:
        {
            if (SIZE_RESTORED == w_param)
            {
                std::thread thr([wnd] {
                    ::smooth_wnd_show(wnd);
                });

            thr.detach();
            }

            break;
        }

        // Устанавливаем соответствующий курсор, когда ...
        // ... пользователь перетаскивает окно.
        case WM_MOVING:
        {
            static const ::HCURSOR cursor{
                ::LoadCursorW(nullptr, IDC_SIZEALL)
            };

            ::SetCursor(cursor);
            break;
        }

        case WM_CREATE:
        {
            caption_font = ::CreateFontW(
                22,                    // высота шрифта;
                0,                     // средняя ширина символа;
                0,                     // угол наклона;
                0,                     // угол ориентации базисной линии;
                FW_NORMAL,             // толщина шрифта;
                FW_DONTCARE,           // описатель параметра курсивного шрифта;
                0,                     // описатель параметра подчеркивания;
                0,                     // описать параметра зачеркивания;
                DEFAULT_CHARSET,       // идентификатор набора символов;
                OUT_TT_ONLY_PRECIS,    // точность вывода;
                CLIP_CHARACTER_PRECIS, // точность отсечения;
                PROOF_QUALITY,         // качество вывода;
                FF_MODERN,             // шаг между символами шрифта и семейство;
                L"Segoe UI"            // имя гарнитуры шрифта.
            );

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
                L"Segoe UI"
            );

            // кнопка "Закрыть" (красного цвета):
            const ::HWND close_btn{ ::CreateWindowExW(
                0u,
                L"Button",
                nullptr,
                WS_CHILD | WS_VISIBLE | BS_BITMAP,
                220,
                5,
                25,
                25,
                wnd,
                reinterpret_cast<::HMENU>(::CONTROLS_IDS::CLOSE_BUTTON),
                inst,
                nullptr)
            };

            // подгружаем иконку кнопки из ресурсов:
            const ::HBITMAP close_btn_bmp{ ::LoadBitmapW(
                inst,
                MAKEINTRESOURCEW(IDB_CLOSE_BUTTON))
            };

            // устанавливаем иконку кнопке:
            ::SendMessageW(
                close_btn,
                BM_SETIMAGE,
                IMAGE_BITMAP,
                reinterpret_cast<::LPARAM>(close_btn_bmp)
            );

            // Выгружаем битмап из памяти:
            ::DeleteObject(close_btn_bmp);

            // "Кнопочная" процедура для отработки необходимых сообщений:
            ::SetWindowSubclass(
                close_btn,
                control_btn_proc,
                0u,
                0u
            );

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
                reinterpret_cast<::HMENU>(::CONTROLS_IDS::MINIMIZE_BUTTON),
                inst,
                nullptr)
            };

            // Подгружаем битмап для кнопки из ресурсов:
            const ::HBITMAP minimize_btn_bmp{ ::LoadBitmapW(
                inst,
                MAKEINTRESOURCEW(IDB_MINIMIZE_BUTTON))
            };

            // Устанавливаем загруженный битмап кнопке:
            ::SendMessageW(
                minimize_btn,
                BM_SETIMAGE,
                IMAGE_BITMAP,
                reinterpret_cast<::LPARAM>(minimize_btn_bmp)
            );

            // Выгружаем битмап из памяти:
            ::DeleteObject(minimize_btn_bmp);

            // Аналогично с кнопкой "Свернуть":
            ::SetWindowSubclass(
                minimize_btn,
                control_btn_proc,
                0u,
                0u
            );

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
                nullptr
            );

            ::SendMessageW(
                pass_amnt_form,
                WM_SETFONT,
                reinterpret_cast<::WPARAM>(text_font),
                0
            );

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
                nullptr
            );

            ::SendMessageW(
                pass_len_form,
                WM_SETFONT,
                reinterpret_cast<::WPARAM>(text_font),
                0
            );

            // Создание чекбоксов:
            std::size_t h_offset{ 100u };

            for (std::uint8_t i{ 3u }; i <= 7u; i++)
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
                    nullptr
                );

                ::CheckDlgButton(wnd, i, BST_CHECKED);
                h_offset += 22u;
            }

			// Кнопка "Сгенерировать пароли":
			const ::HWND generate_btn{ CreateWindowExW(
				0u,
				L"Button",
				nullptr,
				WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
				10,
				220,
				230,
				40,
				wnd,
				reinterpret_cast<::HMENU>(::CONTROLS_IDS::GENERATE_BUTTON),
				inst,
				nullptr) };

			::SetWindowSubclass(
				generate_btn,
				generate_btn_proc,
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
				reinterpret_cast<::HMENU>(::CONTROLS_IDS::INFO_BUTTON),
				inst,
				nullptr) };

			::SetWindowSubclass(
				info_btn,
				info_btn_proc,
				0u,
				0u);

			break;
		}

		case WM_COMMAND:
		{
			// Младшее слово определяет идентификатор пункта меню
			switch (LOWORD(w_param))
			{
				case ::CONTROLS_IDS::CLOSE_BUTTON:
				{
					const ::HWND info_wnd{ ::FindWindowW(
						info_wnd_class_name,
						info_wnd_caption) };

					if (nullptr != info_wnd)
					{
						std::thread thr {
							smooth_wnd_hide,
							info_wnd,
							true
						};

						thr.detach();
					}

					::smooth_wnd_hide(wnd, true);

					// Выгружаем шрифты:
					::DeleteObject(caption_font);
					::DeleteObject(text_font);
				
					::ExitProcess(EXIT_SUCCESS);
				}

				case ::CONTROLS_IDS::MINIMIZE_BUTTON:
				{
					const ::HWND info_wnd{ ::FindWindowW(
						info_wnd_class_name,
						info_wnd_caption) };

					if (nullptr != info_wnd)
					{
						std::thread thr(
							smooth_wnd_hide,
							info_wnd,
							true);

						thr.detach();
					}

					::smooth_wnd_hide(wnd, false);
					::ShowWindow(wnd, SW_MINIMIZE);

					break;
				}

				case ::CONTROLS_IDS::GENERATE_BUTTON:
				{
					// Проверим значения чекбоксов кроме ЧБ "Избегать повторений"
					std::size_t true_values_amnt{ 0u };

					for (std::size_t i{ 0u }; i < 4u; i++)
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
							L"Галочка должна стоять как минимум у одного из чекбоксов.\n"
                            L"Чекбокс \"Избегать повторений\" не считается.",
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
							L"Поля «Кол-во паролей» и «Длина каждого пароля».\n"
							L"Введённые значения не соответствуют ограничениям.\n"
							L"Минимальное кол-во паролей: 1, максимальное: 4096.\n"
							L"Минимальная длина пароля: 4, максимальная: 256.",
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
								L"Введеное значение не соответствует ограничениям.\n"
								L"Минимальное кол-во паролей: 1, максимальное: 4096.",
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
								L"Введеное значение не соответствует ограничениям\n"
								L"Минимальная длина пароля: 4, максимальная: 256.",
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
					const auto passwords{ ::generate_passwords(
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
						::wsprintfW(generation_time_buf, L"%s", L"< 1");

					(0u != write_time) ?
						::wsprintfW(write_time_buf, L"%u", write_time) :
						::wsprintfW(write_time_buf, L"%s", L"< 1");

					::wsprintfW(notification,
						L"Пароли записаны в файл \"%s\"\nВремя генерации: %s мс.\nВремя записи в файл: %s мс.",
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
			
				case ::CONTROLS_IDS::INFO_BUTTON:
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
					static ::ATOM reg_res{ ::reg_wnd_class(
						info_wnd_class_name,
						about_app_wnd_proc,
						inst)
					};

					if (!reg_res)
					{
						::handle_error(GetLastError());
					}

					static const uint32_t
						info_wnd_w{ 250u },
						info_wnd_h{ info_wnd_w };

					const
						std::pair<uint32_t,
						uint32_t> pos_pair{
						::get_wnd_center(
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

					// Если не удалось создать окно
					if (nullptr == info_wnd)
					{
						::handle_error(GetLastError());
					}

					::UpdateWindow(info_wnd);
					::ShowWindow(info_wnd, SW_SHOWNORMAL);

					std::thread thr{ [info_wnd] {
						smooth_wnd_show(info_wnd);
					}};

					thr.detach();
					break;
				}
			}

			// Обработка чекбоксов:
			if (LOWORD(w_param) >= 3u && LOWORD(w_param) <= 7u) // [3; 7] - чекбоксы
			{
				const ::UINT result { ::IsDlgButtonChecked(wnd, LOWORD(w_param)) }; // получаем текущее состояние

                ::CheckDlgButton(wnd, LOWORD(w_param), !result); // меняем визуальное состояние
				check_box_vals[LOWORD(w_param) - 3u] = !result; // присваиваем соответствующее значение массиву логических значений чекбоксов
			}
		}

		case WM_PAINT:
		{
			::PAINTSTRUCT ps{};
			::HDC hdc{ BeginPaint(wnd, &ps) };

			::SelectObject(hdc, caption_font);
			::SetBkColor(hdc, RGB(29u, 29u, 29u));
			::SetTextColor(hdc, RGB(255u, 255u, 255u));

			// Отображение текста:
			::TextOutW(hdc, 10, 6, L"Генератор паролей", ::lstrlenW(L"Генератор паролей"));

			::SelectObject(hdc, text_font);

			::TextOutW(hdc, 10u, 52u, L"Кол-во паролей:", ::lstrlenW(L"Кол-во паролей:"));
			::TextOutW(hdc, 10u, 74u, L"Длина каждого пароля:", ::lstrlenW(L"Длина каждого пароля:"));

			::TextOutW(hdc, 30u, 103u, L"Использовать цифры", ::lstrlenW(L"Использовать цифры"));
			::TextOutW(hdc, 30u, 125u, L"Использовать заглавные буквы", ::lstrlenW(L"Использовать заглавные буквы"));
			::TextOutW(hdc, 30u, 147u, L"Использовать строчные буквы", ::lstrlenW(L"Использовать строчные буквы"));
			::TextOutW(hdc, 30u, 169u, L"Использовать символы", ::lstrlenW(L"Использовать символы"));
			::TextOutW(hdc, 30u, 191u, L"Избегать повторений", ::lstrlenW(L"Избегать повторений"));

			EndPaint(wnd, &ps);
			break;
		}

		case WM_CTLCOLORSTATIC:
		{
			// Инициализируем этот описатель (дескриптор) только 1 раз.
			// В противном случае будет утечка памяти.
			static const HBRUSH hBrush{ CreateSolidBrush(RGB(29u, 29u, 29u)) };
			return reinterpret_cast<LRESULT>(hBrush);
		}

		// Трюк ниже позволяет перетаскивать окно за любую его часть.
		case WM_LBUTTONDOWN: // Ловим нажатие ЛКМ.
			// Отправляемое сообщение буквально значит "нажата ЛКМ на заголовке окна".
			::SendMessageW(wnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
			break;

		default:
			return ::DefWindowProcW(wnd, msg, w_param, l_param);
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
