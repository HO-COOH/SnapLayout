#include "pch.h"
#include "ThumbnailVisual.h"
#include "DwmPrivateApi.h"
#include <dcomp.h>


#define DWM_TNP_FREEZE            0x100000
#define DWM_TNP_ENABLE3D          0x4000000
#define DWM_TNP_DISABLE3D         0x8000000
#define DWM_TNP_FORCECVI          0x40000000
#define DWM_TNP_DISABLEFORCECVI   0x80000000

static SIZE queryWindowThumbnailSize(HWND hwnd)
{
	SIZE size;
	winrt::check_hresult(DwmPrivateApi::DwmpQueryWindowThumbnailSourceSize(hwnd, false, &size));
	return size;
}

static winrt::com_ptr<IDCompositionVisual2> createWindowThumbnail(
	HWND source, 
	HWND target, 
	IDCompositionDesktopDevice* compositionDevice,
	SIZE thumbnailSize)
{
	DWM_THUMBNAIL_PROPERTIES thumbnailProperties
	{
		.dwFlags = DWM_TNP_SOURCECLIENTAREAONLY | DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION | DWM_TNP_RECTSOURCE | DWM_TNP_OPACITY | DWM_TNP_ENABLE3D,
		.rcDestination = RECT{0, 0, thumbnailSize.cx, thumbnailSize.cy},
		.rcSource = RECT{0, 0, thumbnailSize.cx, thumbnailSize.cy},
		.opacity = 255,
		.fVisible = true,
		.fSourceClientAreaOnly = false
	};

	winrt::com_ptr<IDCompositionVisual2> windowThumbnailVisual;
	HTHUMBNAIL hThumbnail;
	winrt::check_hresult(
		DwmPrivateApi::DwmpCreateSharedThumbnailVisual(
			target,
			source,
			2,
			&thumbnailProperties,
			compositionDevice,
			windowThumbnailVisual.put_void(),
			&hThumbnail
		)
	);
	return windowThumbnailVisual;
}

static winrt::Windows::UI::Composition::Visual directCompositionVisualToVisual(
	IDCompositionDesktopDevice* dcompDevice,
	IDCompositionVisual2* dcompVisual,
	SIZE size)
{
	winrt::com_ptr<IDCompositionVisual2> directCompositionVisualContainer;
	winrt::check_hresult(dcompDevice->CreateVisual(directCompositionVisualContainer.put()));
	winrt::check_hresult(directCompositionVisualContainer->AddVisual(dcompVisual, true, nullptr));

	auto compositionVisual = directCompositionVisualContainer.as<winrt::Windows::UI::Composition::Visual>();
	compositionVisual.Size({
		static_cast<float>(size.cx),
		static_cast<float>(size.cy)
	});
	return compositionVisual;
}

static winrt::Windows::UI::Composition::Visual createCompositionVisualForHwnd(
	HWND hwnd, 
	HWND target,
	IDCompositionDesktopDevice* dcompDevice)
{
	SIZE thumbnailVisualSize = queryWindowThumbnailSize(hwnd);
	auto thumbnailVisual = createWindowThumbnail(hwnd, target, dcompDevice, thumbnailVisualSize);
	return directCompositionVisualToVisual(dcompDevice, thumbnailVisual.get(), thumbnailVisualSize);
}

ThumbnailVisual::ThumbnailVisual(HWND hwnd, HWND target, IDCompositionDesktopDevice* dcompDevice) : 
	Visual{createCompositionVisualForHwnd(hwnd, target, dcompDevice)}
{

}
