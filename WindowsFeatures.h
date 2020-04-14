#pragma once

// 2-ой параметр: закрывается окно или нет (может сворачиваться)
void	smoothWindowApprearance(HWND hWnd, bool isClosing); // плавное появление
void	smoothWindowHide(HWND hWnd, bool isClosing); // плавное скрытие

// Функция, регистрирующая класс окна:
ATOM	registerWindowClass(const char* winClassName, WNDPROC windowProcedure, HINSTANCE hInstance);

// Получение координат для окна относительно его ширины и высоты:
std::pair<uint16_t, uint16_t>	getWindowCenterCoordinates(uint16_t xSize, uint16_t ySize);