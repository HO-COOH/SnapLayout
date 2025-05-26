#include "pch.h"
#include "MouseHookDll.h"
#include <optional>

static std::optional<MouseHookDll> gInstance;

void MouseHookDll::Set(HWND targetMessageHwnd, bool& hasLButtonDown)
{
	gInstance.emplace();
	gInstance->installHook(targetMessageHwnd, &hasLButtonDown);
}
