#include "pch.h"
#include "MouseHookDll.h"
#include <optional>

static std::optional<MouseHookDll> gInstance;

void MouseHookDll::Set(HWND targetMessageHwnd)
{
	gInstance.emplace();
	reinterpret_cast<InstallHookFunc>(GetProcAddress(gInstance->lib.get(), "InstallHook"))(targetMessageHwnd, &gInstance->hasLButtonDown);
}

void MouseHookDll::Unset()
{
	reinterpret_cast<UnsetHookFunc>(GetProcAddress(gInstance->lib.get(), "UnsetHook"))();
	gInstance.reset();
}

bool MouseHookDll::HasLButtonDown()
{
	return gInstance->hasLButtonDown;
	return true;
}
