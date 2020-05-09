#include "Main.h"
#include "MainWindow.h"

#include "WindowsFeatures.h"
#include "AboutWindow.h"
#include "InfoButton.h"

#include "ControlButtons.h"
#include "GenerationButton.h"


LRESULT CALLBACK mainWindowProcedure(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
void handleError(DWORD uIdEror);


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	LPCWSTR szMainWindowClassName = L"Password Generator";
	LPCWSTR szMainWindowCaption = L"Password Generator [by DolgorukovGTA]";

	// Текущий дескриптор приложения:
	hInstance = GetModuleHandle(NULL);

	// Проверка ниже позволит избежать запуска нескольких экземпляров приложения:
	HANDLE hInstanceMutex = CreateMutex(NULL, TRUE, szMainWindowCaption);
	if (hInstanceMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		HWND hExistingWindow = FindWindow(szMainWindowClassName, szMainWindowCaption);
		if (hExistingWindow) {
			MessageBox(NULL, L"Приложение Password Generator уже открыто!", L"Уведомление:", MB_ICONWARNING);
			SetForegroundWindow(hExistingWindow);

		}

		CloseHandle(hInstanceMutex);
		return EXIT_SUCCESS;
	}

	if (!registerWindowClass(szMainWindowClassName, mainWindowProcedure, hInstance))
	{
		handleError(GetLastError());
	}

	// Размеры окна:
	const int32_t
		mainWindowWidth = 250, // ширина
		mainWindowHeight = 315; // высота

	std::pair<int32_t, int32_t> posPair = getWindowCenterCoordinates(mainWindowWidth, mainWindowHeight);

	HWND hMainWindow = CreateWindowEx(WS_EX_LAYERED, szMainWindowClassName,
		szMainWindowCaption, WS_POPUP, posPair.first, posPair.second,
		mainWindowWidth, mainWindowHeight, NULL, NULL, hInstance, NULL);

	// Анимация появления окна в сообщении WM_SIZE
	// На текущий момент окно не видно

	if (hMainWindow == NULL)
	{
		handleError(GetLastError());
	}

	ShowWindow(hMainWindow, SW_SHOWNORMAL);
	UpdateWindow(hMainWindow);

	MSG msg{};

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK mainWindowProcedure(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static bool checkBoxStatuses[5] = { true, true, true, true, true };

	static HWND
		hEditPasswordsAmount = NULL,
		hEditPasswordsLength = NULL;

	// Объявляем переменные под шрифты, которые будут задействованы в сообщение WM_PAINT
	// Шрифты инициализируются в сообщении WM_CREATE
	static HFONT
		hCaptionFont = NULL,
		hTextFont = NULL;

	static LPCWSTR
		aboutWindowClassName = L"About Program Window",
		aboutWindowCaption = L"О программе Password Generator [by DolgorukovGTA]";

	static HINSTANCE hCurrentInstance = GetModuleHandle(NULL);

	switch (uMsg)
	{
		// Плавная анимация во время разворачивания и появления в первый раз окна:
	case WM_SIZE:
	{
		if (wParam == SIZE_RESTORED)
		{
			std::thread thr([hWindow]
			{
				smoothWindowApprearance(hWindow);
			});

			thr.detach();
		}

		break;
	}

	// Устанавливаем соответствующий курсор, когда пользователь ...
	// ... перетаскивает окно
	case WM_MOVING:
	{
		static HCURSOR hCursor = LoadCursor(NULL, IDC_SIZEALL);
		SetCursor(hCursor);
		break;
	}

	case WM_CREATE:
	{
		hCaptionFont = CreateFont(22, 0, 0, 0, FW_NORMAL, FW_DONTCARE, 0, 0,
			DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_CHARACTER_PRECIS,
			PROOF_QUALITY, FF_MODERN, L"Segoe UI");

		hTextFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FW_DONTCARE, 0, 0,
			DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_CHARACTER_PRECIS,
			PROOF_QUALITY, FF_MODERN, L"Segoe UI");

		// Кнопка "Закрыть" (красного цвета):
		HWND hCloseButton = CreateWindow(L"Button", NULL,
			WS_CHILD | WS_VISIBLE | BS_BITMAP,
			220, 5, 25, 25, hWindow,
			reinterpret_cast<HMENU>(CLOSE_BUTTON), hCurrentInstance, NULL);

		// Подгружаем иконку для кнопки из ресурсов:
		HBITMAP hCloseButtonBitmap = LoadBitmap(hCurrentInstance,
			MAKEINTRESOURCE(IDB_CLOSE_BUTTON));

		// Устанавливаем иконку кнопке:
		SendMessage(hCloseButton, BM_SETIMAGE, IMAGE_BITMAP,
			reinterpret_cast<LPARAM>(hCloseButtonBitmap));

		// Выгружаем битмап из памяти:
		DeleteObject(hCloseButtonBitmap);

		// "Кнопочная" процедура (рукопис.) для отработки необходимых сообщений:
		SetWindowSubclass(hCloseButton, controlButtonProcedure, 0, 0);

		// Кнопка "Свернуть" (жёлтого цвета):
		HWND hMinimizeButton = CreateWindow(L"Button", NULL,
			WS_CHILD | WS_VISIBLE | BS_BITMAP,
			190, 5, 25, 25, hWindow,
			reinterpret_cast<HMENU>(MINIMIZE_BUTTON), hCurrentInstance, NULL);

		// Подгружаем битмап для кнопки из ресурсов:
		HBITMAP hMinimizeButtonBitmap = LoadBitmap(hCurrentInstance,
			MAKEINTRESOURCE(IDB_MINIMIZE_BUTTON));

		// Устанавливаем загруженный битмап кнопке:
		SendMessage(hMinimizeButton, BM_SETIMAGE, IMAGE_BITMAP,
			reinterpret_cast<LPARAM>(hMinimizeButtonBitmap));

		// Выгружаем битмап из памяти:
		DeleteObject(hMinimizeButtonBitmap);

		// Аналогично с кнопкой "Свернуть":
		SetWindowSubclass(hMinimizeButton, controlButtonProcedure, 0, 0);

		// Поле ввода "Количество паролей":
		hEditPasswordsAmount = CreateWindow(L"Edit", L"64",
			WS_CHILD | WS_VISIBLE | ES_NUMBER,
			195, 49, 40, 20, hWindow, NULL, hCurrentInstance, NULL);

		SendMessage(hEditPasswordsAmount, WM_SETFONT, reinterpret_cast<WPARAM>(hTextFont), NULL);

		// Поле ввода "Длина каждого пароля":
		hEditPasswordsLength = CreateWindow(L"Edit", L"32",
			WS_CHILD | WS_VISIBLE | ES_NUMBER,
			195, 72, 40, 20, hWindow, NULL, hCurrentInstance, NULL);

		SendMessage(hEditPasswordsLength, WM_SETFONT, reinterpret_cast<WPARAM>(hTextFont), NULL);

		// Создание чекбоксов:
		uint16_t uHeight = 100;
		for (uint8_t i = 3; i <= 7; i++)
		{
			CreateWindow(L"Button", NULL,
				WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
				10, uHeight, 15, 25, hWindow,
				reinterpret_cast<HMENU>(i), hCurrentInstance, NULL);

			CheckDlgButton(hWindow, i, BST_CHECKED);
			uHeight += 22;
		}

		// Кнопка "Сгенерировать пароли":
		HWND hGenerationButton = CreateWindow(L"Button", NULL,
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			10, 220, 230, 40, hWindow,
			reinterpret_cast<HMENU>(GENERATE_BUTTON), hCurrentInstance, NULL);

		SetWindowSubclass(hGenerationButton, generationButtonProcedure, NULL, NULL);

		// Кнопка "О программе":
		HWND hInfoButton = CreateWindow(L"Button", NULL,
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			10, 265, 230, 40, hWindow,
			reinterpret_cast<HMENU>(INFO_BUTTON), hCurrentInstance, NULL);

		SetWindowSubclass(hInfoButton, infoButtonProcedure, NULL, NULL);
		break;
	}

	case WM_COMMAND:
	{
		// Младшее слово определяет идентификатор пункта меню
		switch (LOWORD(wParam))
		{
		case CLOSE_BUTTON:
		{
			HWND hAboutWindow = FindWindow(aboutWindowClassName, aboutWindowCaption);

			if (hAboutWindow)
			{
				std::thread thr(smoothWindowHide, hAboutWindow, true);
				thr.detach();
			}

			smoothWindowHide(hWindow, true);

			// Выгружаем шрифты:
			DeleteObject(hCaptionFont);
			DeleteObject(hTextFont);
			ExitProcess(EXIT_SUCCESS);
		}

		case MINIMIZE_BUTTON:
		{
			HWND hAboutWindow = FindWindow(aboutWindowClassName, aboutWindowCaption);

			if (hAboutWindow)
			{
				std::thread thr(smoothWindowHide, hAboutWindow, true);
				thr.detach();
			}

			smoothWindowHide(hWindow, false);

			ShowWindow(hWindow, SW_MINIMIZE);
			break;
		}

		case GENERATE_BUTTON:
		{

			// Проверим значения чекбоксов кроме ЧБ "Избегать повторений"
			uint8_t uTrueValuesAmount = 0;
			for (uint8_t i = 0; i < 4; i++)
			{
				if (checkBoxStatuses[i])
				{
					++uTrueValuesAmount;
				}
			}

			// Используемые кавычки: « »
			if (uTrueValuesAmount == 0)
			{
				MessageBox(hWindow,
					L"Галочка должна стоять как минимум у одного из чекбоксов. Чекбокс «Избегать повторений» не считается.",
					L"Ошибка:", MB_ICONERROR);
				break;
			}

			// Считываем информацию с полей "Количество паролей" и ...
			// ... "Длина каждого пароля":
			char
				passAmountBuf[5]{},
				passLenBuf[5]{};

			GetWindowTextA(hEditPasswordsAmount, passAmountBuf, 4);
			GetWindowTextA(hEditPasswordsLength, passLenBuf, 4);

			int32_t passAmount = atoi(passAmountBuf);
			int32_t passLen = atoi(passLenBuf);

			if (passAmount == 0 && passLen == 0)
			{
				MessageBox(hWindow,
					L"Поля «Кол-во паролей» и «Длина каждого пароля». Введённые значения не соответствуют ограничениям.\nМинимальное кол-во паролей: 5, максимальное: 4096.\nМинимальная длина пароля: 4, максимальная: 256.",
					L"Ошибка ввода:", MB_ICONERROR);
				break;
			}

			else
			{
				if (passAmount == 0 || passAmount < 5 || passAmount > 4096)
				{
					MessageBox(hWindow,
						L"Введеное значение не соответствует ограничениям.\nМинимальное кол-во паролей: 5, максимальное: 4096.",
						L"Ошибка ввода:", MB_ICONERROR);
					break;
				}

				if (passLen == 0 || passLen < 4 || passLen > 256)
				{
					MessageBox(hWindow,
						L"Введеное значение не соответствует ограничениям\nМинимальная длина пароля: 4, максимальная: 256.",
						L"Ошибка ввода:", MB_ICONERROR);
					break;
				}
			}

			SYSTEMTIME st{};
			GetLocalTime(&st);
			wchar_t fileName[MAX_PATH]{};
			wsprintf(fileName, L"(P = %u, L = %u) %u.%u.%u %u-%u-%u-%u.txt",
				passAmount, passLen, st.wMonth, st.wDay, st.wYear, st.wHour,
				st.wMinute, st.wSecond, st.wMilliseconds);

			OPENFILENAME ofn{};
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWindow;
			ofn.hInstance = NULL;
			// Строка ниже действительно должна заканчиваться 2-умя нуль-терминаторами:
			ofn.lpstrFilter = L"Текстовый файл (*.txt)\0.txt\0";
			ofn.lpstrCustomFilter = NULL;
			ofn.nMaxCustFilter = 256;
			ofn.nFilterIndex = 0;
			ofn.lpstrFile = fileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.lpstrTitle = NULL;
			ofn.Flags = NULL;
			ofn.nFileOffset = 0;
			ofn.nFileExtension = 0;
			ofn.lpstrDefExt = NULL;
			ofn.lCustData = NULL;
			ofn.lpfnHook = NULL;
			ofn.lpTemplateName = NULL;

			if (!GetSaveFileName(&ofn))
			{
				break;
			}

			uint32_t uStartTime = GetTickCount();

			// Функция ниже возвращает динамический массив с сгенерированными паролями, время их генерации
			// Указатель на указатель, т.к массив состоит из Си-строк
			char** pPasswordsArray = generatePasswords(passAmount, passLen, checkBoxStatuses);

			uint32_t uGenerationTime = GetTickCount() - uStartTime; // засекаем время генерации

			uStartTime = GetTickCount(); // обновляем счётчик
			HANDLE hWriteFile = CreateFile(ofn.lpstrFile, FILE_WRITE_DATA, NULL,
				NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			if (hWriteFile == INVALID_HANDLE_VALUE)
			{
				handleError(GetLastError());
				break;
			}

			// кол-во паролей максимально может достигать четырёхзначного числа

			char tmpBuf[5]{};
			size_t uWriteBufferSize = 0;

			for (uint16_t i = 0; i < passAmount; i++)
			{
				sprintf(tmpBuf, "%i", i + 1);

				// y + 1 + 1 + x + 1 + 1
				uWriteBufferSize = strlen(tmpBuf) + 1 + 1 + strlen(pPasswordsArray[i]) + 1;

				// В последней строке не нужно писать \n, поэтому строка становится на 1 символ меньше

				if (i != passAmount - 1) // Учитываем сообщение выше
				{
					++uWriteBufferSize;
				}

				char* pWriteBuffer = new char[uWriteBufferSize];

				sprintf(pWriteBuffer, (i != passAmount - 1) ? "%i: %s\n" : "%i: %s",
					i + 1, pPasswordsArray[i]);

				BOOL writeResult = WriteFile(hWriteFile, pWriteBuffer, uWriteBufferSize - 1, 0, NULL);
				if (!writeResult)
				{
					wchar_t szMsg[256]{0};
					wsprintf(szMsg,
						L"Не удалось записать строку №%i в файл.\nЕсли Вы нажмёте «Повторить попытку» (Retry), то приложение попытается вновь записать в файл эту строку.\nЕсли нажмёте «Отмена» (Cancel), то приложение остановит запись в файл.", i + 1);
					int32_t msgBoxResult = MessageBox(hWindow, szMsg, L"Уведомление:", MB_RETRYCANCEL | MB_ICONERROR | MB_APPLMODAL);
					if (msgBoxResult == IDRETRY)
					{
						--i;
					}

					else
					{
						delete[] pWriteBuffer;
						CloseHandle(hWriteFile);
						return 0;
					}

				}

				delete[] pWriteBuffer;
			}

			CloseHandle(hWriteFile);

			uint32_t uWritingTime = GetTickCount() - uStartTime; // засекаем время записи в файл

			// Массивы, в которые будут писаться время записи и генерации:
			wchar_t geBuf[10]{};
			wchar_t wrBuf[10]{};

			(uGenerationTime != 0) ? wsprintf(geBuf, L"%u", uGenerationTime) : wsprintf(geBuf, L"< 1");
			(uWritingTime != 0) ? wsprintf(wrBuf, L"%u", uWritingTime) : wsprintf(wrBuf, L"< 1");

			wchar_t szNotificationMsg[320]{};
			wsprintf(szNotificationMsg,
				L"Пароли записаны в файл '%s'\nВремя генерации: %s мс\nВремя записи в файл: %s мс",
				ofn.lpstrFile, geBuf, wrBuf);

			MessageBox(hWindow, szNotificationMsg, L"Уведомление:", MB_OK);

			// Освобождение выделенной памяти под массив с паролями:
			for (uint16_t i = 0; i < passAmount; i++)
				delete[] pPasswordsArray[i];
			delete[] pPasswordsArray;

			break;
		}

		case INFO_BUTTON:
		{
			HWND hAboutWindow = FindWindow(aboutWindowClassName, aboutWindowCaption);
			if (hAboutWindow)
			{
				SetForegroundWindow(hAboutWindow);
				break;
			}

			// Процедура окна "О программе" находится в файле AboutWindow.cpp
			static ATOM regResult = registerWindowClass(aboutWindowClassName, aboutWindowProcedure, hCurrentInstance);
			if (regResult == 0)
			{
				handleError(GetLastError());
			}

			static const int32_t
				aboutWindowWeight = 250,
				aboutWindowHeight = 250;

			std::pair<int32_t, int32_t> posPair =
				getWindowCenterCoordinates(aboutWindowWeight, aboutWindowHeight);

			// Окно "О программе"
			hAboutWindow = CreateWindowEx(WS_EX_LAYERED,
				aboutWindowClassName, aboutWindowCaption,
				WS_POPUP, posPair.first, posPair.second,
				aboutWindowWeight, aboutWindowHeight, NULL,
				NULL, hCurrentInstance, NULL);

			if (hAboutWindow == NULL)
			{
				handleError(GetLastError());
			}

			UpdateWindow(hAboutWindow);
			ShowWindow(hAboutWindow, SW_SHOWNORMAL);

			std::thread thr([hAboutWindow]
			{
				smoothWindowApprearance(hAboutWindow);
			});

			thr.detach();
			break;
		}
		}

		// Обработка чекбоксов:
		if (LOWORD(wParam) >= 3 && LOWORD(wParam) <= 7) // [3; 7] - чекбоксы
		{
			BOOL result = IsDlgButtonChecked(hWindow, LOWORD(wParam)); // получаем текущее состояние
			CheckDlgButton(hWindow, LOWORD(wParam), result ? BST_UNCHECKED : BST_CHECKED); // меняем визуальное состояние
			checkBoxStatuses[LOWORD(wParam) - 3] = result ? false : true; // присваиваем соответствующее значение массиву логических значений чекбоксов
		}
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps{};
		HDC hdc = BeginPaint(hWindow, &ps);

		SelectObject(hdc, hCaptionFont);
		SetBkColor(hdc, RGB(29, 29, 29));
		SetTextColor(hdc, RGB(255, 255, 255));

		// Отображение текста:
		TextOut(hdc, 10, 6, L"Генератор паролей", strlen("Генератор паролей"));

		SelectObject(hdc, hTextFont);

		TextOut(hdc, 10, 52, L"Кол-во паролей:", lstrlen(L"Кол-во паролей:"));
		TextOut(hdc, 10, 74, L"Длина каждого пароля:", lstrlen(L"Длина каждого пароля:"));

		TextOut(hdc, 30, 103, L"Использовать цифры", lstrlen(L"Использовать цифры"));
		TextOut(hdc, 30, 125, L"Использовать заглавные буквы", lstrlen(L"Использовать заглавные буквы"));
		TextOut(hdc, 30, 147, L"Использовать строчные буквы", lstrlen(L"Использовать строчные буквы"));
		TextOut(hdc, 30, 169, L"Использовать символы", lstrlen(L"Использовать символы"));
		TextOut(hdc, 30, 191, L"Избегать повторений", lstrlen(L"Избегать повторений"));

		EndPaint(hWindow, &ps);
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
		SendMessage(hWindow, WM_NCLBUTTONDOWN, HTCAPTION, NULL);
		break;

	default:
		return DefWindowProc(hWindow, uMsg, wParam, lParam);
	}

	return 0;
}


void handleError(DWORD uIdEror)
{
	wchar_t szMsg[64]{};
	wsprintf(szMsg, L"Приложение завершает работу с ошибкой. ID ошибки: %u.", uIdEror);
	MessageBox(NULL, szMsg, L"Ошибка:", MB_ICONERROR);
	ExitProcess(EXIT_FAILURE);
}
