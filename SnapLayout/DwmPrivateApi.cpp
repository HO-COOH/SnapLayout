#include "pch.h"
#include "DwmPrivateApi.h"

DwmPrivateApi::DwmPrivateApi()
{
	auto hlib = lib.get();
	lDwmpQueryThumbnailType = reinterpret_cast<DwmpQueryThumbnailTypeFunc>(GetProcAddress(hlib, MAKEINTRESOURCEA(114)));
	lDwmpCreateSharedThumbnailVisual = reinterpret_cast<DwmpCreateSharedThumbnailVisualFunc>(GetProcAddress(hlib, MAKEINTRESOURCEA(147)));
	lDwmpQueryWindowThumbnailSourceSize = reinterpret_cast<DwmpQueryWindowThumbnailSourceSizeFunc>(GetProcAddress(hlib, MAKEINTRESOURCEA(162)));
	
	lDwmpCreateSharedVirtualDesktopVisual = reinterpret_cast<DwmpCreateSharedVirtualDesktopVisualFunc>(GetProcAddress(hlib, MAKEINTRESOURCEA(163)));
	lDwmpUpdateSharedVirtualDesktopVisual = reinterpret_cast<DwmpUpdateSharedVirtualDesktopVisualFunc>(GetProcAddress(hlib, MAKEINTRESOURCEA(164)));
}

DwmPrivateApi& DwmPrivateApi::Instance()
{
	static DwmPrivateApi s_instance;
	return s_instance;
}

HRESULT WINAPI DwmPrivateApi::DwmpCreateSharedThumbnailVisual(IN HWND hwndDestination, IN HWND hwndSource, IN DWORD dwThumbnailFlags, IN DWM_THUMBNAIL_PROPERTIES* pThumbnailProperties, IN VOID* pDCompDevice, OUT VOID** ppVisual, OUT PHTHUMBNAIL phThumbnailId)
{
	return Instance().lDwmpCreateSharedThumbnailVisual(hwndDestination, hwndSource, dwThumbnailFlags, pThumbnailProperties, pDCompDevice, ppVisual, phThumbnailId);
}

HRESULT WINAPI DwmPrivateApi::DwmpQueryWindowThumbnailSourceSize(IN HWND hwndSource, IN BOOL fSourceClientAreaOnly, OUT SIZE* pSize)
{
	return Instance().lDwmpQueryWindowThumbnailSourceSize(hwndSource, fSourceClientAreaOnly, pSize);
}

HRESULT WINAPI DwmPrivateApi::DwmpQueryThumbnailType(IN HTHUMBNAIL hThumbnailId, OUT THUMBNAIL_TYPE* thumbType)
{
	return Instance().lDwmpQueryThumbnailType(hThumbnailId, thumbType);
}

HRESULT WINAPI DwmPrivateApi::DwmpCreateSharedVirtualDesktopVisual(IN HWND hwndDestination, IN VOID* pDCompDevice, OUT VOID** ppVisual, OUT PHTHUMBNAIL phThumbnailId)
{
	return Instance().lDwmpCreateSharedVirtualDesktopVisual(hwndDestination, pDCompDevice, ppVisual, phThumbnailId);
}

HRESULT WINAPI DwmPrivateApi::DwmpUpdateSharedVirtualDesktopVisual(IN HTHUMBNAIL hThumbnailId, IN HWND* phwndsInclude, IN DWORD chwndsInclude, IN HWND* phwndsExclude, IN DWORD chwndsExclude, OUT RECT* prcSource, OUT SIZE* pDestinationSize)
{
	return Instance().lDwmpUpdateSharedVirtualDesktopVisual(hThumbnailId, phwndsInclude, chwndsInclude, phwndsExclude, chwndsExclude, prcSource, pDestinationSize);
}
