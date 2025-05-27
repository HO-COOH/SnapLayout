#include "pch.h"
#include "LayoutFunctions.h"
#include <ShellScalingApi.h>
#include "DpiUtils.hpp"
#include <winrt/Microsoft.UI.Windowing.h>
#include "AcrylicVisualWindow.xaml.h"
#pragma comment(lib, "Shcore.lib")

LayoutResult LayoutImpl(LayoutResult layoutResult, HMONITOR monitor, HWND draggedWindow)
{
	LayoutResult windowPlacement = layoutResult;

	//leave some padding
	if (layoutResult.height >= 0.99f)
		layoutResult.height -= 0.01f;
	layoutResult.x = std::clamp(layoutResult.x, 0.005f, 0.995f);
	layoutResult.y = std::clamp(layoutResult.y, 0.005f, 0.995f);

	// set acrylic visual window
	MONITORINFO info{ .cbSize = sizeof(info) };
	winrt::check_bool(GetMonitorInfo(monitor, &info));

	auto const& rcWork = info.rcWork;
	layoutResult.x *= (rcWork.right - rcWork.left);
	windowPlacement.x *= (rcWork.right - rcWork.left);
	layoutResult.width *= (rcWork.right - rcWork.left);
	windowPlacement.width *= (rcWork.right - rcWork.left);
	layoutResult.y *= (rcWork.bottom - rcWork.top);
	windowPlacement.y *= (rcWork.bottom - rcWork.top);
	layoutResult.height *= (rcWork.bottom - rcWork.top);
	windowPlacement.height *= (rcWork.bottom - rcWork.top);


	UINT dpiX, dpiY;
	winrt::check_hresult(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY));

	layoutResult.x = UnscaleForDpi(layoutResult.x, dpiX);
	layoutResult.width = UnscaleForDpi(layoutResult.width, dpiX);
	layoutResult.y = UnscaleForDpi(layoutResult.y, dpiX);
	layoutResult.height = UnscaleForDpi(layoutResult.height, dpiX);

	auto acrylicWindow = winrt::get_self<winrt::SnapLayout::implementation::AcrylicVisualWindow>(winrt::SnapLayout::implementation::AcrylicVisualWindow::Instance);
	acrylicWindow->AppWindow().MoveAndResize({ 0, 0, rcWork.right - rcWork.left, rcWork.bottom - rcWork.top });
	acrylicWindow->SetVisualPosition(layoutResult, draggedWindow, dpiX);
	//Set acrylic window after mainWindow in z-order


	return windowPlacement;
}
