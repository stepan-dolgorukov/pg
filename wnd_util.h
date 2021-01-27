#pragma once

#include <cstdint>
#include <utility> // std::pair
#include <Windows.h>

void smooth_wnd_show(const ::HWND wnd);			  // плавное появление

void smooth_wnd_hide(
	const ::HWND wnd,
	const bool close); // плавное скрытие

::ATOM reg_wnd_class(
	::LPCWSTR class_name,
	::WNDPROC wnd_proc,
	::HINSTANCE inst); // регистрация класса окна

// Получение координат для окна относительно его ширины и высоты:
std::pair<std::size_t, std::size_t> get_wnd_center(
	const std::size_t x,
	const std::size_t y);