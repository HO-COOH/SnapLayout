#pragma once
#include "BaseWindow.hpp"
#include <winrt/Windows.UI.Composition.h>
#include "FreezableThumbnailVisual.h"
#include <optional>

class ThumbnailVisualContainerWindow : public BaseWindow<ThumbnailVisualContainerWindow, L"ThumbnailVisualContainerWindow">
{
	winrt::Windows::UI::Composition::Compositor compositor{ nullptr };
	winrt::Windows::UI::Composition::ContainerVisual root{ nullptr };
	std::optional<FreezableThumbnailVisual> visual;
	HWND currentVisualHwnd{};
	winrt::Microsoft::Graphics::Canvas::CanvasDevice canvasDevice;
	winrt::Windows::UI::Composition::CompositionGraphicsDevice compositionGraphicsDevice{ nullptr };
	winrt::Windows::UI::Composition::CompositionDrawingSurface surface{ nullptr };
	
	ThumbnailVisualContainerWindow();
public:
	void SetVisual(HWND sourceHwnd, winrt::Windows::Foundation::Numerics::float2 animationAnchor);
	void StartAnimation();
	static ThumbnailVisualContainerWindow& Instance();

	void Show();
	void Hide();

	void Move(int x, int y);
};

