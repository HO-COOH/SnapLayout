#pragma once
#include <winrt/Windows.UI.Composition.h>
#include "BaseWindow.hpp"

struct IDCompositionDesktopDevice;

class ThumbnailVisual : public winrt::Windows::UI::Composition::Visual
{

public:
	ThumbnailVisual(HWND hwnd, HWND target, IDCompositionDesktopDevice* dcompDevice);
};

