#pragma once
#include <wil/resource.h>

enum WINDOWCOMPOSITIONATTRIB
{
    WCA_UNDEFINED = 0x0,
    WCA_NCRENDERING_ENABLED = 0x1,
    WCA_NCRENDERING_POLICY = 0x2,
    WCA_TRANSITIONS_FORCEDISABLED = 0x3,
    WCA_ALLOW_NCPAINT = 0x4,
    WCA_CAPTION_BUTTON_BOUNDS = 0x5,
    WCA_NONCLIENT_RTL_LAYOUT = 0x6,
    WCA_FORCE_ICONIC_REPRESENTATION = 0x7,
    WCA_EXTENDED_FRAME_BOUNDS = 0x8,
    WCA_HAS_ICONIC_BITMAP = 0x9,
    WCA_THEME_ATTRIBUTES = 0xA,
    WCA_NCRENDERING_EXILED = 0xB,
    WCA_NCADORNMENTINFO = 0xC,
    WCA_EXCLUDED_FROM_LIVEPREVIEW = 0xD,
    WCA_VIDEO_OVERLAY_ACTIVE = 0xE,
    WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 0xF,
    WCA_DISALLOW_PEEK = 0x10,
    WCA_CLOAK = 0x11,
    WCA_CLOAKED = 0x12,
    WCA_ACCENT_POLICY = 0x13,
    WCA_FREEZE_REPRESENTATION = 0x14,
    WCA_EVER_UNCLOAKED = 0x15,
    WCA_VISUAL_OWNER = 0x16,
    WCA_HOLOGRAPHIC = 0x17,
    WCA_EXCLUDED_FROM_DDA = 0x18,
    WCA_PASSIVEUPDATEMODE = 0x19,
    WCA_LAST = 0x1A,
};

struct WINDOWCOMPOSITIONATTRIBDATA
{
	WINDOWCOMPOSITIONATTRIB Attrib;
	void* pvData;
	DWORD cbData;
};

class User32PrivateApi
{
	using SetWindowCompositionAttributeFunc = BOOL(WINAPI*)(
		IN HWND hwnd,
		IN WINDOWCOMPOSITIONATTRIBDATA* pwcad);

    wil::unique_hmodule lib{ LoadLibrary(L"user32.dll") };

    SetWindowCompositionAttributeFunc lSetWindowCompositionAttribute{ reinterpret_cast<SetWindowCompositionAttributeFunc>(GetProcAddress(lib.get(), "SetWindowCompositionAttribute")) };

    static User32PrivateApi& Instance();
public:
    static BOOL WINAPI SetWindowCompositionAttribute(
        IN HWND hwnd,
        IN WINDOWCOMPOSITIONATTRIBDATA* pwcad
    );
};

