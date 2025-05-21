#include "pch.h"
#include "User32PrivateApi.h"

User32PrivateApi& User32PrivateApi::Instance()
{
    static User32PrivateApi s_instance;
    return s_instance;
}

BOOL WINAPI User32PrivateApi::SetWindowCompositionAttribute(IN HWND hwnd, IN WINDOWCOMPOSITIONATTRIBDATA* pwcad)
{
    return Instance().lSetWindowCompositionAttribute(hwnd, pwcad);
}
