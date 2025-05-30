#include "pch.h"
#include "WindowDragEventListener.h"
#include "ThumbnailVisualContainerWindow.h"
#include "AcrylicVisualWindow.xaml.h"
#include "MainWindow.xaml.h"
#include <dwmapi.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include "DebugHelper.hpp"
#include <optional>
#include "DpiUtils.hpp"
#include "MouseHookDll.h"

static std::optional<WindowDragEventListener> g_eventListener;

static bool isWindowResizable(HWND hwnd)
{
	return GetWindowLongPtr(hwnd, GWL_STYLE) & WS_THICKFRAME;
}

VOID WindowDragEventListener::WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF)
        return;

    switch (event)
    {
	    case EVENT_SYSTEM_MOVESIZESTART: return onMoveSizeStart(event, hwnd, idObject, idChild, dwEventThread, dwmsEventTime);
		case EVENT_SYSTEM_MOVESIZEEND: return onMoveSizeEnd(event, hwnd, idObject, idChild, dwEventThread, dwmsEventTime);
		case EVENT_OBJECT_CREATE: return onObjectCreate(event, hwnd, idObject, idChild, dwEventThread, dwmsEventTime);
		case EVENT_OBJECT_DESTROY: return onObjectDestroy(event, hwnd, idObject, idChild, dwEventThread, dwmsEventTime);
        default:    return;
    }
}

void WindowDragEventListener::onMoveSizeStart(DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    //Non-resizable window should not trigger snap layout
    if (!isWindowResizable(hwnd) || !MouseHookDll::HasLButtonDown())
        return;

    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);
    if (static DWORD const pidCurrent = GetCurrentProcessId(); pid == pidCurrent)
        return;

    winrt::check_bool(GetCursorPos(&g_eventListener->g_draggedWindowCursorPoint));
    winrt::check_bool(ScreenToClient(hwnd, &g_eventListener->g_draggedWindowCursorPoint));
    g_eventListener->g_hwndTracked = hwnd;
    winrt::check_bool(GetWindowRect(hwnd, &g_eventListener->g_rcInitial));
    winrt::SnapLayout::implementation::MainWindow::GetInstance()->OnShow(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST));
    DebugLog("Move/Resize started on window");

    ThumbnailVisualContainerWindow::Instance().SetVisual(g_eventListener->g_hwndTracked, g_eventListener->g_draggedWindowCursorPoint);
}

void WindowDragEventListener::onMoveSizeEnd(DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    if (hwnd != g_eventListener->g_hwndTracked)
        return;


    RECT rcFinal;
    GetWindowRect(hwnd, &rcFinal);
    if (memcmp(&rcFinal, &g_eventListener->g_rcInitial, sizeof(RECT)) != 0) {
        DebugLog("Window was moved.\n");
    }
    g_eventListener->g_hwndTracked = NULL;
    //winrt::SnapLayout::implementation::MainWindow::GetInstance()->OnDismiss();
}

void WindowDragEventListener::onObjectCreate(DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    if (IsWindow(hwnd))
    {
        wchar_t title[256];
        auto count = GetWindowText(hwnd, title, std::size(title));
        title[count] = L'\0 ';
        //DebugLog(L"Window created: {}\n", title);
    }
}

void WindowDragEventListener::onObjectDestroy(DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    //DebugLog("Window destroyed.\n");
}

WindowDragEventListener::WindowDragEventListener(WindowDragEventListener::private_ctor_t, HWINEVENTHOOK hook) : g_hEventHook{ hook }
{
    if (!hook)
        throw std::runtime_error{ "Failed to hook window event" };
}

void WindowDragEventListener::Set()
{
    g_eventListener.emplace(WindowDragEventListener::private_ctor_t{}, SetWinEventHook(
        EVENT_SYSTEM_MOVESIZESTART, EVENT_OBJECT_DESTROY,
        NULL, WinEventProc,
        0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
    ));
}

void WindowDragEventListener::Unset()
{
    g_eventListener.reset();
}

HWND WindowDragEventListener::GetDraggedWindow()
{
    assert(HasWindowDragging());
    return g_eventListener->g_hwndTracked;
}

bool WindowDragEventListener::HasWindowDragging()
{
    return g_eventListener->g_hwndTracked != NULL;
}

POINT WindowDragEventListener::GetDraggedWindowPointOffset()
{
    return g_eventListener->g_draggedWindowCursorPoint;
}

void WindowDragEventListener::HideDraggedWindow(POINT cursorPoint, UINT dpi)
{
    ThumbnailVisualContainerWindow::Instance().StartAnimation();
    ShowWindow(g_eventListener->g_hwndTracked, SW_HIDE);
    DebugLog(L"Window moved offscreen\n");
}

