#include "pch.h"
#include "FreezableThumbnailVisual.h"
#include "Interop.hpp"
#include <winrt/Microsoft.Graphics.Canvas.h>

FreezableThumbnailVisual::FreezableThumbnailVisual(
	HWND hwnd, 
	HWND target, 
	winrt::Windows::UI::Composition::Compositor const& compositor,
	winrt::Windows::UI::Composition::CompositionDrawingSurface const& surface,
	winrt::Microsoft::Graphics::Canvas::CanvasDevice const& canvasDevice)
	: ThumbnailVisual{ hwnd, target, compositor.as<IDCompositionDesktopDevice>().get() }
{
	auto captureItem = winrt::Windows::Graphics::Capture::GraphicsCaptureItem::CreateFromVisual(*this);
	framePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::Create(
		canvasDevice,
		winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
		1,
		captureItem.Size()
	);

	session = framePool.CreateCaptureSession(captureItem);
	session.StartCapture();
	framePool.FrameArrived([this, compositor, surface, canvasDevice, hwnd](
		winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool const& sender,
		winrt::Windows::Foundation::IInspectable const&)
		{
			auto frame = sender.TryGetNextFrame();
			auto frameSurface = frame.Surface();
			{
				auto canvasBitmap = winrt::Microsoft::Graphics::Canvas::CanvasBitmap::CreateFromDirect3D11Surface(canvasDevice, frameSurface);
				auto session = winrt::Microsoft::Graphics::Canvas::UI::Composition::CanvasComposition::CreateDrawingSession(surface);
				session.Clear(winrt::Windows::UI::Colors::Transparent());
				session.DrawImage(canvasBitmap);
			}
			auto spriteVisual = compositor.CreateSpriteVisual();
			auto const size = frame.ContentSize();
			spriteVisual.Size({ static_cast<float>(size.Width), static_cast<float>(size.Height) });
			spriteVisual.Brush(compositor.CreateSurfaceBrush(surface));
			static_cast<winrt::Windows::UI::Composition::Visual&>(*this) = spriteVisual;

			session.Close();
			framePool.Close();
			ShowWindow(hwnd, SW_HIDE);
		});
}
