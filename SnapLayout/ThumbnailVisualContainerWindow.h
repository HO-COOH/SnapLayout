#pragma once
#include "BaseWindow.hpp"
#include <winrt/Windows.UI.Composition.h>

class ThumbnailVisualContainerWindow : public BaseWindow<ThumbnailVisualContainerWindow, L"ThumbnailVisualContainerWindow">
{
	winrt::Windows::UI::Composition::Compositor compositor{ nullptr };
	winrt::Windows::UI::Composition::ContainerVisual root{ nullptr };


	winrt::Microsoft::Graphics::Canvas::CanvasDevice canvasDevice;
	winrt::Windows::UI::Composition::CompositionGraphicsDevice compositionGraphicsDevice{ nullptr };
	winrt::Windows::UI::Composition::CompositionDrawingSurface surface{ nullptr };
	ThumbnailVisualContainerWindow();
public:
	void SetVisual(HWND sourceHwnd, RECT windowPosition);
	static ThumbnailVisualContainerWindow& Instance();
	void Hide();
};

