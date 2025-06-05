#pragma once

class Monitor
{
	HMONITOR m_monitor;
public:
	Monitor(HMONITOR monitor);

	MONITORINFO GetInfo() const;
	UINT GetDpi() const;

	static Monitor FromWindow(HWND hwnd, DWORD flags = MONITOR_DEFAULTTONEAREST);
};
