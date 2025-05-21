#pragma once
#include <Windows.h>
#include <dwmapi.h>
#include <wil/resource.h>

class DwmPrivateApi
{
public:
    enum THUMBNAIL_TYPE
    {
        TT_DEFAULT = 0x0,
        TT_SNAPSHOT = 0x1,
        TT_ICONIC = 0x2,
        TT_BITMAPPENDING = 0x3,
        TT_BITMAP = 0x4
    };
private:

    using DwmpCreateSharedThumbnailVisualFunc = HRESULT(WINAPI*)(
        IN HWND hwndDestination,
        IN HWND hwndSource,
        IN DWORD dwThumbnailFlags,
        IN DWM_THUMBNAIL_PROPERTIES* pThumbnailProperties,
        IN VOID* pDCompDevice,
        OUT VOID** ppVisual,
        OUT PHTHUMBNAIL phThumbnailId);

    using DwmpQueryWindowThumbnailSourceSizeFunc = HRESULT(WINAPI*)(
        IN HWND hwndSource,
        IN BOOL fSourceClientAreaOnly,
        OUT SIZE* pSize);

    using DwmpQueryThumbnailTypeFunc = HRESULT(WINAPI*)(
        IN HTHUMBNAIL hThumbnailId,
        OUT THUMBNAIL_TYPE* thumbType);

    //pre-cobalt/pre-iron
    using DwmpCreateSharedVirtualDesktopVisualFunc = HRESULT(WINAPI*)(
        IN HWND hwndDestination,
        IN VOID* pDCompDevice,
        OUT VOID** ppVisual,
        OUT PHTHUMBNAIL phThumbnailId);

    //cobalt/iron (20xxx+)
    //No changes except for the function name.
    using DwmpCreateSharedMultiWindowVisualFunc = HRESULT(WINAPI*)(
        IN HWND hwndDestination,
        IN VOID* pDCompDevice,
        OUT VOID** ppVisual,
        OUT PHTHUMBNAIL phThumbnailId);

    //pre-cobalt/pre-iron
    using DwmpUpdateSharedVirtualDesktopVisualFunc = HRESULT(WINAPI*)(
        IN HTHUMBNAIL hThumbnailId,
        IN HWND* phwndsInclude,
        IN DWORD chwndsInclude,
        IN HWND* phwndsExclude,
        IN DWORD chwndsExclude,
        OUT RECT* prcSource,
        OUT SIZE* pDestinationSize);

    //cobalt/iron (20xxx+)
    //Change: function name + new DWORD parameter.
    //Pass "1" in dwFlags. Feel free to explore other flags.
    using DwmpUpdateSharedMultiWindowVisualFunc = HRESULT(WINAPI*)(
        IN HTHUMBNAIL hThumbnailId,
        IN HWND* phwndsInclude,
        IN DWORD chwndsInclude,
        IN HWND* phwndsExclude,
        IN DWORD chwndsExclude,
        OUT RECT* prcSource,
        OUT SIZE* pDestinationSize,
        IN DWORD dwFlags);

    //------------------------- Getting functions
    DwmpQueryThumbnailTypeFunc lDwmpQueryThumbnailType;
    DwmpCreateSharedThumbnailVisualFunc lDwmpCreateSharedThumbnailVisual;
    DwmpQueryWindowThumbnailSourceSizeFunc lDwmpQueryWindowThumbnailSourceSize;

    //PRE-IRON
    DwmpCreateSharedVirtualDesktopVisualFunc lDwmpCreateSharedVirtualDesktopVisual;
    DwmpUpdateSharedVirtualDesktopVisualFunc lDwmpUpdateSharedVirtualDesktopVisual;

    //20xxx+
    //DwmpCreateSharedMultiWindowVisualFunc lDwmpCreateSharedMultiWindowVisual;
    //DwmpUpdateSharedMultiWindowVisualFunc lDwmpUpdateSharedMultiWindowVisual;

    wil::unique_hmodule lib{ LoadLibrary(L"dwmapi.dll") };

    DwmPrivateApi();

    static DwmPrivateApi& Instance();
public:
    static HRESULT WINAPI DwmpCreateSharedThumbnailVisual(
        IN HWND hwndDestination,
        IN HWND hwndSource,
        IN DWORD dwThumbnailFlags,
        IN DWM_THUMBNAIL_PROPERTIES* pThumbnailProperties,
        IN VOID* pDCompDevice,
        OUT VOID** ppVisual,
        OUT PHTHUMBNAIL phThumbnailId);

    static HRESULT WINAPI DwmpQueryWindowThumbnailSourceSize(
        IN HWND hwndSource,
        IN BOOL fSourceClientAreaOnly,
        OUT SIZE* pSize);

    static HRESULT WINAPI DwmpQueryThumbnailType(
        IN HTHUMBNAIL hThumbnailId,
        OUT THUMBNAIL_TYPE* thumbType);

    //pre-cobalt/pre-iron
    static HRESULT WINAPI DwmpCreateSharedVirtualDesktopVisual(
        IN HWND hwndDestination,
        IN VOID* pDCompDevice,
        OUT VOID** ppVisual,
        OUT PHTHUMBNAIL phThumbnailId);

    //cobalt/iron (20xxx+)
    //No changes except for the function name.
    //static HRESULT WINAPI DwmpCreateSharedMultiWindowVisual(
    //    IN HWND hwndDestination,
    //    IN VOID* pDCompDevice,
    //    OUT VOID** ppVisual,
    //    OUT PHTHUMBNAIL phThumbnailId);

    //pre-cobalt/pre-iron
    static HRESULT WINAPI DwmpUpdateSharedVirtualDesktopVisual(
        IN HTHUMBNAIL hThumbnailId,
        IN HWND* phwndsInclude,
        IN DWORD chwndsInclude,
        IN HWND* phwndsExclude,
        IN DWORD chwndsExclude,
        OUT RECT* prcSource,
        OUT SIZE* pDestinationSize);

    //cobalt/iron (20xxx+)
    //Change: function name + new DWORD parameter.
    //Pass "1" in dwFlags. Feel free to explore other flags.
    //static HRESULT WINAPI DwmpUpdateSharedMultiWindowVisual(
    //    IN HTHUMBNAIL hThumbnailId,
    //    IN HWND* phwndsInclude,
    //    IN DWORD chwndsInclude,
    //    IN HWND* phwndsExclude,
    //    IN DWORD chwndsExclude,
    //    OUT RECT* prcSource,
    //    OUT SIZE* pDestinationSize,
    //    IN DWORD dwFlags);
};
