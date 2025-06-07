#include "pch.h"
#include "LayoutFunctions.h"
#include <ShellScalingApi.h>
#include "DpiUtils.hpp"
#include <winrt/Microsoft.UI.Windowing.h>
#include "AcrylicVisualWindow.xaml.h"
#include "MonitorWrapper.h"
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
	auto const& rcWork = Monitor{ monitor }.GetInfo().rcWork;
	ConvertLayoutToMonitorWindowPlacement(layoutResult, rcWork);
	ConvertLayoutToMonitorWindowPlacement(windowPlacement, rcWork);

	auto const dpiX = Monitor{ monitor }.GetDpi();
	layoutResult.UnscaleForDpi(dpiX);

	winrt::get_self<winrt::SnapLayout::implementation::AcrylicVisualWindow>(winrt::SnapLayout::implementation::AcrylicVisualWindow::Instance)->SetVisualPosition(layoutResult, draggedWindow, dpiX, rcWork);

	return windowPlacement;
}

void ConvertLayoutToMonitorWindowPlacement(LayoutResult& layoutResult, RECT rcWork)
{
	auto const width = rcWork.right - rcWork.left;
	layoutResult.x *= width;
	layoutResult.width *= width;
	auto const height = rcWork.bottom - rcWork.top;
	layoutResult.y *= height;
	layoutResult.height *= height;
}

void ConvertLayoutToMonitorWindowPlacement(LayoutResult& layoutResult, HMONITOR monitor)
{
	ConvertLayoutToMonitorWindowPlacement(layoutResult, Monitor{ monitor }.GetInfo().rcWork);
}
