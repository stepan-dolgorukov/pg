#pragma once

#include <Windows.h>

::LRESULT __stdcall generate_btn_proc(
	::HWND wnd,
	::UINT msg,
	::WPARAM wparam,
	::LPARAM lparam,
	::UINT_PTR sub_class,
	::DWORD_PTR ref_data);