#pragma once

class Monitor
{
	HMONITOR m_monitor;
public:
	Monitor(HMONITOR monitor);

	[[nodiscard]] MONITORINFO GetInfo() const;
	[[nodiscard]] UINT GetDpi() const;

	static Monitor FromWindow(HWND hwnd, DWORD flags = MONITOR_DEFAULTTONEAREST);
};
