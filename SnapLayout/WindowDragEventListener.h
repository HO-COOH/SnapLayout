#pragma once

class WindowDragEventListener
{
	static HWINEVENTHOOK g_hEventHook;

	static RECT g_rcInitial;
	static RECT g_beforeHide;
	static VOID CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd,
		LONG idObject, LONG idChild, DWORD dwEventThread,
		DWORD dwmsEventTime);
public:
	static HWND g_hwndTracked;
	static void Set();
	
	//We cannot use ShowWindow(..., SW_HIIDE) to hide because that will make window thumbnail stop working.
	//We can instead just move the dragged window to an offscreen position
	static void HideDraggedWindow();


	static void ShowDraggedWindow();
	static bool HasWindowDragging();
};

