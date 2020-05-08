#pragma once

#define _CRT_SECURE_NO_WARNINGS

// Необходимо для корректного отображения кастомных контролов:
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "comctl32.lib")

#include <Windows.h>
#include <cstring>
#include <cstdint>
#include <commctrl.h>
#include <thread>
#include "resource.h" // ресурсы проекта

// ID обрабатываемых контров:
enum ControlsID
{
	CLOSE_BUTTON = 1,
	MINIMIZE_BUTTON = 2,
	GENERATE_BUTTON = 8,
	INFO_BUTTON = 9
};