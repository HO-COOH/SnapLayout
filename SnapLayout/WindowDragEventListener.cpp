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
#include "WindowUtils.hpp"

static std::optional<WindowDragEventListener> g_eventListener;

VOID WindowDragEventListener::WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
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

void WindowDragEventListener::onMoveSizeStart(
    [[maybe_unused]] DWORD event, 
    HWND hwnd, 
    [[maybe_unused]] LONG idObject, 
    [[maybe_unused]] LONG idChild, 
    [[maybe_unused]] DWORD dwEventThread, 
    [[maybe_unused]] DWORD dwmsEventTime)
{
    //Non-resizable window should not trigger snap layout
    if (!IsWindowResizable(hwnd) || !MouseHookDll::HasLButtonDown())
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

void WindowDragEventListener::onMoveSizeEnd(
    [[maybe_unused]] DWORD event,
    HWND hwnd,
    [[maybe_unused]] LONG idObject,
    [[maybe_unused]] LONG idChild,
    [[maybe_unused]] DWORD dwEventThread,
    [[maybe_unused]] DWORD dwmsEventTime)
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

void WindowDragEventListener::onObjectCreate(
    [[maybe_unused]] DWORD event,
    HWND hwnd,
    [[maybe_unused]] LONG idObject,
    [[maybe_unused]] LONG idChild,
    [[maybe_unused]] DWORD dwEventThread,
    [[maybe_unused]] DWORD dwmsEventTime)
{
    if (IsWindow(hwnd))
    {
#if defined(_DEBUG) || defined(DEBUG)
        wchar_t title[MAX_PATH]{};
        GetWindowText(hwnd, title, MAX_PATH);
        DebugLog(L"Window created: {}\n", std::wstring_view{ title }.empty() ? L"<<No title>>" : title);
#endif
        if (g_eventListener->g_notifyWindowEvent && IsWindowResizable(hwnd))
        {
            g_eventListener->g_notifyWindowEvent->OnWindowCreated(hwnd);
        }
    }
}

void WindowDragEventListener::onObjectDestroy([[maybe_unused]] DWORD event,
    HWND hwnd,
    [[maybe_unused]] LONG idObject,
    [[maybe_unused]] LONG idChild,
    [[maybe_unused]] DWORD dwEventThread,
    [[maybe_unused]] DWORD dwmsEventTime)
{
    if (g_eventListener->g_notifyWindowEvent)
        g_eventListener->g_notifyWindowEvent->OnWindowDestroyed(hwnd);
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

void WindowDragEventListener::HideDraggedWindow(POINT cursorPoint, [[maybe_unused]] UINT dpi)
{
    ThumbnailVisualContainerWindow::Instance().StartAnimation();
    ShowWindow(g_eventListener->g_hwndTracked, SW_HIDE);
    DebugLog(L"Window moved offscreen\n");
}

void WindowDragEventListener::SubscribeWindowEvent(INotifyWindowEvent* handler)
{
    DebugLog("Subscribe window event\n");
    g_eventListener->g_notifyWindowEvent = handler;
}

void WindowDragEventListener::UnsubscribeWindowEvent(INotifyWindowEvent* handler)
{
    DebugLog("Unsubscribe window event\n");
    g_eventListener->g_notifyWindowEvent = nullptr;
}
