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
	winrt::Windows::UI::Composition::Vector3KeyFrameAnimation shrinkAnimation{ nullptr };
	winrt::Windows::UI::Composition::Vector3KeyFrameAnimation restoreAnimation{ nullptr };
	constexpr static std::chrono::milliseconds duration{ 300 };
	ThumbnailVisualContainerWindow();
public:
	void SetVisual(HWND sourceHwnd, winrt::Windows::Foundation::Numerics::float2 animationAnchor);
	void SetVisual(HWND sourceHwnd, POINT animationAnchor);
	void StartAnimation();
	static ThumbnailVisualContainerWindow& Instance();

	void Show();
	void Hide(bool animation = true);
	[[nodiscard]] HWND GetHwnd() const noexcept;
	void Move(int x, int y);
};

