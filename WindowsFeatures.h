#pragma once

void	smoothWindowApprearance(HWND hWindow); // плавное появление
void	smoothWindowHide(HWND hWindow, bool isClosing); // плавное скрытие

// Функция, регистрирующая класс окна:
ATOM	registerWindowClass(LPCWSTR winClassName, WNDPROC winProcedure, HINSTANCE hInstance);

// Получение координат для окна относительно его ширины и высоты:
std::pair<int32_t, int32_t>	getWindowCenterCoordinates(uint32_t uWidth, uint32_t uHeight);