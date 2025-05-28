#pragma once
#include <winrt/Windows.UI.Composition.h>
#include "BaseWindow.hpp"

struct IDCompositionDesktopDevice;

class ThumbnailVisual : public winrt::Windows::UI::Composition::Visual
{
public:
	ThumbnailVisual(std::nullptr_t) : Visual{ nullptr } {}
	ThumbnailVisual(HWND hwnd, HWND target, IDCompositionDesktopDevice* dcompDevice);
};

