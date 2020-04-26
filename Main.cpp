#include "Main.h"
#include "MainWindow.h"

#include "WindowsFeatures.h"
#include "AboutWindow.h"
#include "InfoButton.h"

#include "ControlButtons.h"
#include "GenerationButton.h"

// Дескриптор приложения:
HINSTANCE hAppInstance;

LRESULT CALLBACK mainWindowProcedure(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	// Неиспользуемые параметры:
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	LPCWSTR szMainWindowClassName = TEXT("Password Generator");
	LPCWSTR szMainWindowCaption = TEXT("Password Generator [by DolgorukovGTA]");

	// Проверка ниже позволит избежать запуска нескольких экземпляров приложения:
	HANDLE hInstanceMutex = CreateMutex(NULL, TRUE, szMainWindowCaption);
	if (!hInstanceMutex || GetLastError() == ERROR_ALREADY_EXISTS) {

		HWND hExistingWindow = FindWindow(szMainWindowClassName, szMainWindowCaption);
		if (hExistingWindow) {
			SetForegroundWindow(hExistingWindow);
		}

		CloseHandle(hInstanceMutex);
		return EXIT_SUCCESS;
	}

	if (FAILED(registerWindowClass(szMainWindowClassName, mainWindowProcedure, hAppInstance))) {
		return EXIT_FAILURE;
	}

	uint16_t mainWindowWidth = 250; // x
	uint16_t mainWindowHeight = 315; // y

	std::pair<uint16_t, uint16_t> posPair = getWindowCenterCoordinates(mainWindowWidth, mainWindowHeight);

	HWND hMainWindow = CreateWindowEx(WS_EX_LAYERED | WS_EX_CONTROLPARENT,
		szMainWindowClassName, szMainWindowCaption, WS_POPUP, posPair.first, posPair.second,
		mainWindowWidth, mainWindowHeight, NULL, NULL, hAppInstance, NULL);

	// Анимация появления окна в сообщении WM_SIZE
	// На текущий момент окно не видно

	if (FAILED(hMainWindow)) {
		return EXIT_FAILURE;
	}

	UpdateWindow(hMainWindow);
	ShowWindow(hMainWindow, SW_SHOWNORMAL);

	MSG msg;

	// Таблица акселераторов, с ней контролы работают шустрее
	HACCEL hAcc = LoadAccelerators(hInstance, NULL);

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator(hMainWindow, hAcc, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK mainWindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool checkBoxStatuses[5] = { true, true, true, true, true };
	static HWND numberOfPasswordsEditControl, passwordsLengthEditControl;

	// Объявляем переменные под шрифты, которые будут задействованы в сообщение WM_PAINT
	// Шрифты инициализируются в сообщении WM_CREATE
	static HFONT captionFont, textFont;

	switch (message) {

	// Когда окно появляется и разворачивается с плавной анимацией 
	case WM_SIZE: {
		if (wParam == SIZE_RESTORED) {
			std::thread thr([window] {
				smoothWindowApprearance(window, false);
			});

			thr.detach();
		}
		
		break;
	}

	// Устанавливаем соответствующий курсор, когда пользователь ...
	// ... перетаскивает окно
	case WM_MOVING: {
		SetCursor(LoadCursor(NULL, IDC_SIZEALL));
		break;
	}

	case WM_CREATE: {
		captionFont = CreateFont(22, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
			DEFAULT_QUALITY, FF_SWISS, TEXT("Segoe UI"));

		textFont = CreateFont(18, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0,
			DEFAULT_QUALITY, FF_SWISS, TEXT("Segoe UI"));

		// Кнопка "Закрыть" (красного цвета):
		HWND closeButton = CreateWindow(TEXT("Button"), NULL,
			WS_CHILD | WS_VISIBLE | BS_BITMAP, 220, 5, 25, 25, 
			window, reinterpret_cast<HMENU>(CLOSE_BUTTON), NULL, NULL);

		// Подгружаем иконку для кнопки из ресурсов:
		HBITMAP closeButtonBitmap = LoadBitmap(GetModuleHandle(NULL),
			MAKEINTRESOURCE(IDB_CLOSE_BUTTON));

		// Устанавливаем иконку кнопке:
		SendMessage(closeButton, BM_SETIMAGE, IMAGE_BITMAP, reinterpret_cast<LPARAM>(closeButtonBitmap));
		DeleteObject(closeButtonBitmap); // Выгружаем иконку из памяти

		// "Кнопочная" процедура (рукопис.) для отработки необходимых сообщений:
		SetWindowSubclass(closeButton, controlButtonProcedure, 0, 0);

		// Кнопка "Свернуть" (жёлтого цвета):
		HWND minimizeButton = CreateWindow(TEXT("Button"), NULL,
			WS_CHILD | WS_VISIBLE | BS_BITMAP, 190, 5, 25, 25, window, 
			reinterpret_cast<HMENU>(MINIMIZE_BUTTON), NULL, NULL);

		// Подгружаем иконку для кнопки из ресурсов:
		HBITMAP minimizeButtonBitmap = LoadBitmap(GetModuleHandle(NULL),
			MAKEINTRESOURCE(IDB_MINIMIZE_BUTTON));

		// Устанавливаем киконку кнопке:
		SendMessage(minimizeButton, BM_SETIMAGE, IMAGE_BITMAP, 
			reinterpret_cast<LPARAM>(minimizeButtonBitmap));

		DeleteObject(minimizeButtonBitmap); // Выгружаем иконку из памяти

		// Аналогично с кнопкой "Закрыть":
		SetWindowSubclass(minimizeButton, controlButtonProcedure, 0, 0);

		// Поле ввода "Количество паролей":
		numberOfPasswordsEditControl = CreateWindow(TEXT("Edit"), TEXT("64"),
			WS_CHILD | WS_VISIBLE | ES_NUMBER, 195, 49, 40, 20, window, NULL, NULL, NULL);

		SendMessage(numberOfPasswordsEditControl, WM_SETFONT, reinterpret_cast<WPARAM>(textFont), 1);

		// Поле ввода "Длина каждого пароля":
		passwordsLengthEditControl = CreateWindow(TEXT("Edit"), TEXT("32"),
			WS_CHILD | WS_VISIBLE | ES_NUMBER, 195, 72, 40, 20, window, NULL, NULL, NULL);

		SendMessage(passwordsLengthEditControl, WM_SETFONT, reinterpret_cast<WPARAM>(textFont), 1);

		// Создание чекбоксов:

		uint8_t height = 100;
		for (uint8_t i = 3; i <= 7; i++) {
			CreateWindow(TEXT("Button"), TEXT("TEST"),
				WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
				10, height, 15, 25, window, reinterpret_cast<HMENU>(i), NULL, NULL);
			CheckDlgButton(window, i, BST_CHECKED);
			height += 22;
		}

		// Кнопка "Сгенерировать пароли":
		HWND generationButton = CreateWindow(TEXT("Button"), NULL,
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 
			10, 220, 230, 40, window, reinterpret_cast<HMENU>(GENERATE_BUTTON), NULL, NULL);

		SetWindowSubclass(generationButton, generationButtonProcedure, NULL, NULL);


		// Кнопка "О программе":
		HWND infoButton = CreateWindow(TEXT("Button"), NULL,
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			10, 265, 230, 40, window, reinterpret_cast<HMENU>(INFO_BUTTON), NULL, NULL);

		SetWindowSubclass(infoButton, infoButtonProcedure, NULL, NULL);
		break;
	}

	case WM_COMMAND: {

		switch (wParam) {

		case CLOSE_BUTTON: {

			HWND aboutWindow = FindWindow(TEXT("About Program Window"), 
				TEXT("О программе Password Generator"));

			if (aboutWindow) {
				std::thread thr(smoothWindowHide, aboutWindow, true);
				thr.detach();
			}

			smoothWindowHide(window, true);

			// Выгружаем шрифты:
			DeleteObject(captionFont);
			DeleteObject(textFont);

			ExitProcess(EXIT_SUCCESS);
		}

		case MINIMIZE_BUTTON: {

			HWND aboutWindow = FindWindow(TEXT("About Program Window"),
				TEXT("О программе Password Generator"));

			if (aboutWindow) {
				std::thread thr(smoothWindowHide, aboutWindow, true);
				thr.detach();
			}

			smoothWindowHide(window, false);

			SendMessage(window, WM_SYSCOMMAND, SC_MINIMIZE, lParam);
			return 0;
		}

		case GENERATE_BUTTON: {

			// Проверим значения чекбоксов кроме ЧБ "Избегать повторений"
			uint8_t trueValuesAmount = 0;
			for (uint8_t i = 0; i < (sizeof(checkBoxStatuses) - 1); i++) {
				if (checkBoxStatuses[i]) {
					++trueValuesAmount;
				}
			}


			// Кавычки: « »
			if (!trueValuesAmount) {
				MessageBox(window,
					TEXT("Галочка должна стоять как минимум у одного из чекбоксов. Чекбокс «Избегать повторений» не считается."),
					TEXT("Нужно быть немного повнимательнее..."), MB_ICONERROR);
				break;
			}

			// Считываем информацию с полей "Количество паролей" и ...
			// ... "Длина каждого пароля":
			char numberOfPasswordsBuffer[5];
			char passwordsLengthBuffer[5];

			GetWindowTextA(numberOfPasswordsEditControl, &numberOfPasswordsBuffer[0],
				sizeof(numberOfPasswordsBuffer));
			GetWindowTextA(passwordsLengthEditControl, &passwordsLengthBuffer[0],
				sizeof(numberOfPasswordsBuffer));

			uint16_t numberOfPasswords = static_cast<uint16_t>(atoi(numberOfPasswordsBuffer));
			uint16_t passwordsLength = static_cast<uint16_t>(atoi(passwordsLengthBuffer));

			if (!numberOfPasswords && !passwordsLength) {
				MessageBox(window, 
					TEXT("Поля «Кол-во паролей» и «Длина каждого пароля»"),
					TEXT("Ошибка ввода!"), MB_ICONERROR);
				break;
			}
			else {
				if (!numberOfPasswords || numberOfPasswords < 5 || numberOfPasswords > 4096) {
					MessageBox(window, 
						TEXT("Введеное значение не соответствует ограничениям.\nМинимальное кол-во паролей: 5, максимальное: 4096."),
						TEXT("Ошибка ввода!"), MB_ICONERROR);
					break;
				}

				if (!passwordsLength || passwordsLength < 4 || passwordsLength > 256) {
					MessageBox(window, 
						TEXT("Введеное значение не соответствует ограничениям\nМинимальная длина пароля: 4, максимальная: 256."),
						TEXT("Ошибка ввода!"), MB_ICONERROR);
					break;
				}
			}


			// Функция ниже возвращает динамический массив с сгенерированными паролями, время их генерации
			// Указатель на указатель, т.к массив состоит из Си-строк
			std::pair<char**, uint16_t> genPair = generatePasswords(numberOfPasswords, passwordsLength, &checkBoxStatuses[0]);
			
			// Создаём ссылки для абстрактности:
			char** &passwordsArray = genPair.first;
			uint16_t &generationTime = genPair.second;

			// Получаем путь к файлу для записи:
			wchar_t* outputFileFullPath = getOutputFileName(numberOfPasswords, passwordsLength);

			// Записываем по полученному пути:
			uint16_t writingTime = writePasswordsArrayToFile(passwordsArray, passwordsLength, 
				numberOfPasswords, outputFileFullPath);

			// Массивы, в которые будут писаться время записи и генерации:
			WCHAR geBuf[10];
			WCHAR wrBuf[10];

			(generationTime != 0) ? wsprintf(geBuf, L"%u", generationTime) : wsprintf(geBuf, L"< 1");
			(writingTime != 0) ? wsprintf(wrBuf, L"%u", writingTime) : wsprintf(wrBuf, L"< 1");

			wchar_t messageWithFilePath[400];
			wsprintf(messageWithFilePath,
				L"Пароли записаны в файл '%s'\nВремя генерации: %s мс\nВремя записи в файл: %s мс",
				outputFileFullPath, geBuf, wrBuf);

			MessageBoxW(window, messageWithFilePath, TEXT("Наслаждайтесь!"), MB_OK);
			;
			// Высвобождения памяти:
			for (uint16_t i = 0; i < numberOfPasswords; i++)
				delete[] passwordsArray[i];

			delete[] passwordsArray;
			break;
		}

		case INFO_BUTTON:

			LPCWSTR aboutWindowClassName = TEXT("About Program Window");
			LPCWSTR aboutWindowCaption = TEXT("О программе Password Generator");

			if (FindWindow(aboutWindowClassName, aboutWindowCaption)) {
				MessageBox(window, TEXT("Окно о программе было открыто ранее."), 
					TEXT("Ошибка!"), MB_ICONERROR);
				break;
			}

			// Процедура окна "О программе" находится в файле AboutWindow.cpp
			if (FAILED(registerWindowClass(aboutWindowClassName, aboutWindowProcedure, NULL))) {
				ExitProcess(EXIT_FAILURE);
			};

			uint16_t aboutWindowWeight = 250;
			uint16_t aboutWindowHeight = 250;

			std::pair<uint16_t, uint16_t> posPair = getWindowCenterCoordinates(aboutWindowWeight, aboutWindowHeight);

			HWND hAboutWindow = CreateWindowEx(WS_EX_LAYERED, 
				aboutWindowClassName, aboutWindowCaption, WS_POPUP, posPair.first, posPair.second, 
				aboutWindowWeight, aboutWindowHeight, NULL, NULL, hAppInstance, NULL); // Окно "О программе"

			if (FAILED(hAboutWindow)) {
				ExitProcess(EXIT_FAILURE);
				break;
			};

			UpdateWindow(hAboutWindow);
			ShowWindow(hAboutWindow, SW_SHOWNORMAL);

			std::thread thr([hAboutWindow] {
				smoothWindowApprearance(hAboutWindow, false);
			});

			thr.detach();
			break;
		}

		// Установка чекбоксам только 2 режима: "вкл" и "выкл"
		if (wParam >= 3 && wParam <= 7) {
			checkBoxStatuses[wParam - 3] = setCheckBoxTrueOrFalse(window, static_cast<uint8_t>(wParam));
		}
	}

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(window, &ps);

		SelectObject(hdc, captionFont);
		SetBkColor(hdc, RGB(29, 29, 29));
		SetTextColor(hdc, RGB(255, 255, 255));

		// Отображение текста:
		TextOut(hdc, 10, 6, TEXT("Генератор паролей"), strlen("Генератор паролей"));

		SelectObject(hdc, textFont);

		TextOut(hdc, 10, 52, TEXT("Кол-во паролей:"), strlen("Кол-во паролей:"));
		TextOut(hdc, 10, 74, TEXT("Длина каждого пароля:"), strlen("Длина каждого пароля:"));

		TextOut(hdc, 30, 103, TEXT("Использовать цифры"), strlen("Использовать цифры"));
		TextOut(hdc, 30, 125, TEXT("Использовать заглавные буквы"), strlen("Использовать заглавные буквы"));
		TextOut(hdc, 30, 147, TEXT("Использовать строчные буквы"), strlen("Использовать строчные буквы"));
		TextOut(hdc, 30, 169, TEXT("Использовать символы"), strlen("Использовать символы"));
		TextOut(hdc, 30, 191, TEXT("Избегать повторений"), strlen("Избегать повторений"));

		EndPaint(window, &ps);
	}

	case WM_CTLCOLORSTATIC: {
		static HBRUSH hBrush = CreateSolidBrush(RGB(29, 29, 29));
		return reinterpret_cast<INT_PTR>(hBrush);
	}

	// Трюк ниже позволяет перетаскивать окно за любую часть окна:
	case WM_LBUTTONDOWN: // Ловим нажатие левой кнопки мыши

		// Отправляемое сообщение буквально значит "Нажата ЛКМ на заголовке окна":
		SendMessage(window, WM_NCLBUTTONDOWN, HTCAPTION, NULL);
		break;

	default:
		return DefWindowProc(window, message, wParam, lParam);
	}

	return 0;
}