#pragma once
#include "ThumbnailVisual.h"
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.UI.Composition.h>
class FreezableThumbnailVisual : public ThumbnailVisual
{
	winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool framePool{ nullptr };
	winrt::Windows::Graphics::Capture::GraphicsCaptureSession session{ nullptr };
	winrt::Windows::UI::Composition::VisualCollection parentVisualCollection{ nullptr };
	winrt::Windows::Foundation::Numerics::float3 centerPoint{};
public:
	FreezableThumbnailVisual(std::nullptr_t) : ThumbnailVisual{ nullptr } {}
	FreezableThumbnailVisual(
		HWND hwnd,
		HWND target,
		winrt::Windows::UI::Composition::Compositor const& compositor,
		winrt::Windows::UI::Composition::CompositionDrawingSurface const& surface,
		winrt::Microsoft::Graphics::Canvas::CanvasDevice const& canvasDevice,
		winrt::Windows::UI::Composition::VisualCollection const& parentVisualCollection
	);

	void CenterPoint(winrt::Windows::Foundation::Numerics::float3 value);
};

