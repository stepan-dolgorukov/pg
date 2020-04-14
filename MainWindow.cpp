#include "Main.h"

#include <string>
#include <random> // (std::random_device, std::mt19937)

bool setCheckBoxTrueOrFalse(HWND window, uint8_t nCheckBox)
{
	if (IsDlgButtonChecked(window, nCheckBox) != BST_CHECKED) {
		CheckDlgButton(window, nCheckBox, BST_UNCHECKED);
		return false;
	}

	return true;
}

std::pair<char**, uint16_t> generatePasswords(uint16_t numberOfPasswords, uint16_t passwordsLength, bool* cbValues)
{
	uint32_t startTime = GetTickCount();

	std::random_device rd;
	std::mt19937 mersenne(rd()); // инициализируем Вихрь Мерсенна случайным стартовым числом

	// Создаём ссылки на значения для абстракции:
	bool
		&useNumbers = cbValues[0],
		&useCapLetters = cbValues[1],
		&useLowLetters = cbValues[2],
		&useExtraSymbols = cbValues[3],
		&avoidRepeat = cbValues[4];

	static const char 
		numbers[11] = "0123456789",
		capLetters[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
		lowLetters[27] = "abcdefghijklmnopqrstuvwxyz",
		symbols[9] = "%*)?@#$~";

	char alphabet[71];

	bool noFirstAdd = false;

	// Алфафит чисел будет хоть как подгружаться первым,
	// проверка на непервое добавление (noFirstAdd) не нужна
	
	if (useNumbers) {
		strcpy(alphabet, numbers);
		noFirstAdd = true;
	}

	if (useCapLetters) {
		if (noFirstAdd) {
			strcat(alphabet, capLetters);
		}
		else {
			strcpy(alphabet, capLetters);
			noFirstAdd = true;
		}
	}

	if (useLowLetters) {
		if (noFirstAdd) {
			strcat(alphabet, lowLetters);
		}
		else {
			strcpy(alphabet, lowLetters);
			noFirstAdd = true;
		}
	}

	if (useExtraSymbols) {
		if (noFirstAdd) {
			strcat(alphabet, symbols);
		}
		else {
			strcpy(alphabet, symbols);
		}
	}

	uint8_t nullTerminatorPosition = 0;

	while (alphabet[nullTerminatorPosition] != '\0') {
		++nullTerminatorPosition;
	}

	char** buf = new char*[numberOfPasswords];

	// Переменные для хранения текущего (сгенерированного) символа ...
	// ... и символа, сгенерированного на предыдущем шаге
	char 
		currentChar = 0, 
		previousChar = 0;

	for (uint16_t i = 0; i < numberOfPasswords; i++) {
		buf[i] = new char[passwordsLength + 1]; // выделение памяти под один пароль

		// Заполняем пароль допустимыми символами:
		for (uint16_t k = 0; k < passwordsLength; k++) {
			
			/* % - деление с остатком,
			rand() возвращает значение между 0 и RAND_MAX (32767) */

			currentChar = alphabet[mersenne() % nullTerminatorPosition];

			if (avoidRepeat && tolower(previousChar) == tolower(currentChar)) { // если символы совпали, то ...
				--k; // ... возвращаемся на шаг назад. На следующем шаге символ будет перезаписан в массив
			}
			else {
				buf[i][k] = currentChar;
				previousChar = currentChar;
			}
		}

		// Добавляем нуль-терминатор в конец каждого пароля:
		buf[i][passwordsLength] = '\0';
	}

	return std::make_pair(buf, static_cast<uint16_t>(GetTickCount() - startTime));
}

char* getOutputFileName(int16_t passwordsAmount, int16_t passwordLength)
{
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);

	// Записываем данные из структуры в название файла:
	char fileName[50];
	sprintf(fileName, "(P = %u, L = %u) %u.%u.%u %u-%u-%u-%u.txt",
		passwordsAmount, passwordLength,
		systemTime.wMonth, systemTime.wDay, systemTime.wYear,
		systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);

	char* outputFilePath = new char[MAX_PATH]; // #define MAX_PATH 260
	GetCurrentDirectory(MAX_PATH, outputFilePath);

	strcat(outputFilePath, "\\");
	strcat(outputFilePath, fileName);
	return outputFilePath; 
}

uint16_t writePasswordsArrayToFile(char** arr, uint16_t passwordLength, uint16_t passwordsAmount, char* fileFullPath) {

	uint32_t startTime = GetTickCount();

	HANDLE outputFile = CreateFileA(fileFullPath, FILE_WRITE_DATA, FILE_SHARE_READ,
		NULL, OPEN_ALWAYS, 0, NULL);

	uint16_t bufSize = passwordLength + 6;

	for (uint16_t i = 0; i < passwordsAmount; i++) {
		char* buf = new char[bufSize];

		sprintf(buf, "%d: %s", i + 1, arr[i]);
		
		if (i != (passwordsAmount - 1)) {
			strcat(buf, "\n");
		}

		uint16_t bytesToWrite = 0;
		while (buf[bytesToWrite] != '\0') {
			++bytesToWrite;
		}

		WriteFile(outputFile, buf, bytesToWrite, 0, NULL);
		delete[] buf;
	}

	// Закрытие файла:
	CloseHandle(outputFile);

	return static_cast<uint16_t>(GetTickCount() - startTime);
}