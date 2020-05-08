#include "Main.h"
#include <random> // (std::random_device, std::mt19937)

char** generatePasswords(int32_t uPasswordsAmount, int32_t uPasswordsLength, bool* cbValues)
{
	std::random_device rd;
	std::mt19937 mersenne(rd()); // инициализируем Вихрь Мерсенна случайным стартовым числом

	// Создаём ссылки на значения для абстрактности:
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

	// 11 + 27 + 27 + 9 = 74
	// 74 - 3 (нули-терминаторы) = 71
	char alphabet[71]{};

	bool noFirstAdd = false;

	// Алфафит чисел будет хоть как подгружаться первым,
	// проверка на непервое добавление (noFirstAdd) не нужна

	if (useNumbers)
	{
		strcpy(alphabet, numbers);
		noFirstAdd = true;
	}
	
	if (useCapLetters) 
	{
		if (noFirstAdd) 
		{
			strcat(alphabet, capLetters);
		}

		else 
		{
			strcpy(alphabet, capLetters);
			noFirstAdd = true;
		}
	}

	if (useLowLetters) {
		if (noFirstAdd) 
		{
			strcat(alphabet, lowLetters);
		}
		else 
		{
			strcpy(alphabet, lowLetters);
			noFirstAdd = true;
		}
	}

	if (useExtraSymbols) 
	{
		if (noFirstAdd) 
		{
			strcat(alphabet, symbols);
		}
		else 
		{
			strcpy(alphabet, symbols);
		}
	}

	uint8_t uNullTerminatorPos = 0;

	while (alphabet[uNullTerminatorPos] != '\0') {
		++uNullTerminatorPos;
	}

	char** buf = new char*[uPasswordsAmount];

	// Переменные для хранения текущего (сгенерированного) символа ...
	// ... и символа, сгенерированного на предыдущем шаге
	char
		currentChar = 0, 
		previousChar = 0;

	for (uint16_t i = 0; i < uPasswordsAmount; i++) 
	{
		buf[i] = new char[uPasswordsLength + 1]; // выделение памяти под один пароль

		// Заполняем пароль допустимыми символами:
		for (uint16_t k = 0; k < uPasswordsLength; k++)
		{
			// % - деление с остатком
			currentChar = alphabet[mersenne() % uNullTerminatorPos];

			if (avoidRepeat && tolower(previousChar) == tolower(currentChar)) 
			{ // если символы совпали, то ...
				--k; // ... возвращаемся на шаг назад. На следующем шаге символ будет перезаписан в массив
			}

			else 
			{
				buf[i][k] = currentChar;
				previousChar = currentChar;
			}
		}

		// Добавляем нуль-терминатор в конец каждого пароля:
		buf[i][uPasswordsLength] = '\0';
	}

	return buf;
}