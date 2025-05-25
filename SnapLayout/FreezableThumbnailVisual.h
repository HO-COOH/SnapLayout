#pragma once
#include "ThumbnailVisual.h"
#include <winrt/Windows.Graphics.Capture.h>

class FreezableThumbnailVisual : public ThumbnailVisual
{
	winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool framePool{ nullptr };
	winrt::Windows::Graphics::Capture::GraphicsCaptureSession session{ nullptr };
public:
	FreezableThumbnailVisual(
		HWND hwnd,
		HWND target,
		winrt::Windows::UI::Composition::Compositor const& compositor,
		winrt::Windows::UI::Composition::CompositionDrawingSurface const& surface,
		winrt::Microsoft::Graphics::Canvas::CanvasDevice const& canvasDevice
	);
};

