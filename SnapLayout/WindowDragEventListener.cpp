#include "pch.h"
#include "WindowDragEventListener.h"
#include "ThumbnailVisualContainerWindow.h"
#include "AcrylicVisualWindow.xaml.h"
#include "MainWindow.xaml.h"
#include <winrt/Microsoft.UI.Windowing.h>

HWND WindowDragEventListener::g_hwndTracked;
HWINEVENTHOOK WindowDragEventListener::g_hEventHook;
RECT WindowDragEventListener::g_rcInitial;
RECT WindowDragEventListener::g_beforeHide;

VOID WindowDragEventListener::WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF)
        return;

    if (event == EVENT_SYSTEM_MOVESIZESTART) {
        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid != GetCurrentProcessId()) {
            g_hwndTracked = hwnd;
            winrt::check_bool(GetWindowRect(hwnd, &g_rcInitial));
            winrt::SnapLayout::implementation::MainWindow::GetInstance()->OnShow();
            OutputDebugStringA("Move/Resize started on window 0x%p\n");
        }
    }
    else if (event == EVENT_SYSTEM_MOVESIZEEND) 
    {
        if (hwnd == g_hwndTracked)
        {
            RECT rcFinal;
            GetWindowRect(hwnd, &rcFinal);
            if (memcmp(&rcFinal, &g_rcInitial, sizeof(RECT)) != 0) {
                OutputDebugStringA("Window was moved.\n");
            }
            g_hwndTracked = NULL;
        }
        //winrt::SnapLayout::implementation::MainWindow::GetInstance()->OnDismiss();
    }
}

void WindowDragEventListener::Set()
{
    g_hEventHook = SetWinEventHook(
        EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND,
        NULL, WinEventProc,
        0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
    );

    if (!g_hEventHook) {
        fprintf(stderr, "Failed to set event hook.\n");
    }
}

void WindowDragEventListener::HideDraggedWindow()
{
    /*Nope, DwmSetWindowAttribute cannot be used to "fix" a thumbnail visual when hiding a window */
    //BOOL value = true;
    //winrt::check_hresult(DwmSetWindowAttribute(WindowDragEventListener::g_hwndTracked, DWMWA_FREEZE_REPRESENTATION, &value, sizeof(value)));
    //ShowWindow(WindowDragEventListener::g_hwndTracked, SW_HIDE);

    //Drag event will bring the window back, also not working
    //winrt::check_bool(GetWindowRect(g_hwndTracked, &g_beforeHide));
    //winrt::check_bool(SetWindowPos(g_hwndTracked, nullptr, 9999, 9999, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE));

    //ThumbnailVisualContainerWindow::Instance().SetVisual(g_hwndTracked, g_rcInitial);
    //ShowWindow(g_hwndTracked, SW_MINIMIZE);
    OutputDebugString(L"Window moved offscreen\n");
}

void WindowDragEventListener::ShowDraggedWindow()
{

}

bool WindowDragEventListener::HasWindowDragging()
{
    return g_hwndTracked != NULL;
}