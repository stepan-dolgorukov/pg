#include <random> // (std::random_device, std::mt19937)
#include <cctype> // std::tolower

char** generate_passwords(
	std::size_t amount,
	std::size_t length,
	bool * const options)
{
	std::random_device rd{};
	std::mt19937 mersenne(rd()); // инициализируем Вихрь Мерсенна случайным стартовым числом

	// Создаём ссылки на значения для абстрактности:
	bool
		&useNumbers = options[0],
		&useCapLetters = options[1],
		&useLowLetters = options[2],
		&useExtraSymbols = options[3],
		&avoidRepeat = options[4];

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

	char** buf = new char*[amount];

	// Переменные для хранения текущего (сгенерированного) символа ...
	// ... и символа, сгенерированного на предыдущем шаге
	char
		currentChar = 0, 
		previousChar = 0;

	for (uint16_t i = 0; i < amount; i++) 
	{
		buf[i] = new char[length + 1]; // выделение памяти под один пароль

		// Заполняем пароль допустимыми символами:
		for (uint16_t k = 0; k < length; k++)
		{
			// % - деление с остатком
			currentChar = alphabet[mersenne() % uNullTerminatorPos];

			if (avoidRepeat && std::tolower(previousChar) == std::tolower(currentChar)) 
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
		buf[i][length] = '\0';
	}

	return buf;
}