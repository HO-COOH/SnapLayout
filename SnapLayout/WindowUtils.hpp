#pragma once
#include <winrt/Microsoft.UI.Xaml.h>
#include <microsoft.ui.xaml.window.h>

[[nodiscard]] inline HWND GetHwnd(winrt::Microsoft::UI::Xaml::Window window)
{
	HWND hwnd{};
	winrt::check_hresult(window.as<IWindowNative>()->get_WindowHandle(&hwnd));
	return hwnd;
}

[[nodiscard]] inline bool IsWindowResizable(HWND hwnd, bool visibleOnly = false)
{
	auto const style = GetWindowLongPtr(hwnd, GWL_STYLE);
	return visibleOnly? 
		(style & WS_THICKFRAME) && (style & WS_VISIBLE) :
		(style & WS_THICKFRAME);
}