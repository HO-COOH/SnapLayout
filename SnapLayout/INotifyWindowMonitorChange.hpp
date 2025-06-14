#pragma once
struct INotifyWindowMonitorChange
{
	virtual void WindowMonitorChanged(HMONITOR monitor) = 0;
};
