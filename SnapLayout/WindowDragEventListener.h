#pragma once
#include <wil/resource.h>

class WindowDragEventListener
{
	struct private_ctor_t{};
	wil::unique_hwineventhook g_hEventHook;

	RECT g_rcInitial;
	RECT g_beforeHide;
	HWND g_hwndTracked;

	static VOID CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd,
		LONG idObject, LONG idChild, DWORD dwEventThread,
		DWORD dwmsEventTime);

	static void onMoveSizeStart(DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);
	static void onMoveSizeEnd(DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);
	static void onObjectCreate(DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);
	static void onObjectDestroy(DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime);
public:
	WindowDragEventListener(private_ctor_t, HWINEVENTHOOK hook);
	static void Set();
	static void Unset();
	static HWND GetDraggedWindow();
	static bool HasWindowDragging();

	//We cannot use ShowWindow(..., SW_HIIDE) to hide because that will make window thumbnail stop working.
	//We can instead just move the dragged window to an offscreen position
	static void HideDraggedWindow(POINT cursorPoint, UINT dpi);
};

