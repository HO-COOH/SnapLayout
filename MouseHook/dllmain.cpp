// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <windows.h>
#include <format>

HHOOK g_hMouseHook = nullptr;
HWND g_targetHwnd = nullptr;
bool g_isOnWindow = false;
bool* g_hasLButtonDown = nullptr;

extern "C" __declspec(dllexport) void UnsetHook() {
    if (g_hMouseHook) {
        UnhookWindowsHookEx(g_hMouseHook);
        g_hMouseHook = nullptr;
    }
}

// Low-level mouse hook procedure
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) 
{
    if (nCode == HC_ACTION) 
    {
        MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;
        POINT pt = pMouse->pt;
        RECT rc;
        GetWindowRect(g_targetHwnd, &rc);

        // Check if mouse is over the target window
        if (PtInRect(&rc, pt))
        {
            g_isOnWindow = true;
            PostMessage(g_targetHwnd, wParam, 0, MAKELPARAM(pt.x, pt.y));
        }
        else if (g_isOnWindow)
        {
            //mouse has left the window
            PostMessage(g_targetHwnd, WM_MOUSELEAVE, 0, 0);
            g_isOnWindow = false;
        }
        
        if (wParam == WM_LBUTTONUP)
        {
            PostMessage(g_targetHwnd, WM_LBUTTONUP, 0, 0);
            *g_hasLButtonDown = false;
        }
        else if (wParam == WM_LBUTTONDOWN)
            *g_hasLButtonDown = true;
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

// Function to install the hook
extern "C" __declspec(dllexport) void InstallHook(HWND hwnd, bool* hasLButtonDown) {
    g_targetHwnd = hwnd;
    g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, GetModuleHandle(nullptr), 0);
	g_hasLButtonDown = hasLButtonDown;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

