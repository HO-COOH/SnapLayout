#pragma once

struct INotifyWindowEvent
{
	virtual void OnWindowCreated(HWND createdWindow) = 0;
	virtual void OnWindowDestroyed(HWND destroyedWindow) = 0;
};
