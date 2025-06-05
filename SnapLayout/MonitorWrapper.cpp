#include "pch.h"
#include "MonitorWrapper.h"
#include <cassert>
#include <ShellScalingApi.h>

Monitor::Monitor(HMONITOR monitor) : m_monitor{ monitor }
{
	assert(m_monitor);
}

MONITORINFO Monitor::GetInfo() const
{
	MONITORINFO info{ .cbSize = sizeof(info) };
	winrt::check_bool(GetMonitorInfo(m_monitor, &info));
	return info;
}

UINT Monitor::GetDpi() const
{
	UINT dpiX, dpiY;
	winrt::check_hresult(GetDpiForMonitor(m_monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY));
	return dpiX;
}

Monitor Monitor::FromWindow(HWND hwnd, DWORD flags)
{
	return Monitor{ MonitorFromWindow(hwnd, flags) };
}
