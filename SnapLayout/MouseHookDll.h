#pragma once

#include <wil/resource.h>
#include <Windows.h>

class MouseHookDll
{
	wil::unique_hmodule lib{ LoadLibrary(L"../../MouseHook.dll") };
	
	using InstallHookFunc = void(*)(HWND, bool*);
	using UnsetHookFunc = void(*)();

	InstallHookFunc installHook = reinterpret_cast<InstallHookFunc>(GetProcAddress(lib.get(), "InstallHook"));
public:
	static void Set(HWND targetMessageHwnd, bool& hasLButtonDown);
};

