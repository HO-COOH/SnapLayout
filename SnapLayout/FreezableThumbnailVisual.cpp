#include "pch.h"
#include "FreezableThumbnailVisual.h"
#include "Interop.hpp"
#include <winrt/Microsoft.Graphics.Canvas.h>
#include <DpiUtils.hpp>
FreezableThumbnailVisual::FreezableThumbnailVisual(
	HWND hwnd, 
	HWND target, 
	winrt::Windows::UI::Composition::Compositor const& compositor,
	winrt::Windows::UI::Composition::CompositionDrawingSurface const& surface,
	winrt::Microsoft::Graphics::Canvas::CanvasDevice const& canvasDevice,
	winrt::Windows::UI::Composition::VisualCollection const& parentVisualCollection)
	: ThumbnailVisual{ hwnd, target, compositor.as<IDCompositionDesktopDevice>().get() },
	parentVisualCollection{parentVisualCollection}
{
	auto captureItem = winrt::Windows::Graphics::Capture::GraphicsCaptureItem::CreateFromVisual(*this);
	framePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::Create(
		canvasDevice,
		winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
		2,
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
			//auto const dpi = GetDpiForWindow(hwnd);
			auto spriteVisual = compositor.CreateSpriteVisual();
			auto const size = frame.ContentSize();
			spriteVisual.Size({ static_cast<float>(size.Width), static_cast<float>(size.Height) });
			//spriteVisual.Size({ UnscaleForDpi(size.Width, dpi), UnscaleForDpi(size.Height, dpi)});
			spriteVisual.Brush(compositor.CreateSurfaceBrush(surface));
			this->StopAnimation(L"Scale");
			spriteVisual.CenterPoint(centerPoint);
			this->parentVisualCollection.RemoveAll();
			this->parentVisualCollection.InsertAtTop(spriteVisual);
			static_cast<winrt::Windows::UI::Composition::Visual&>(*this) = spriteVisual;
			session.Close();
			framePool.Close();
		});
}

void FreezableThumbnailVisual::CenterPoint(winrt::Windows::Foundation::Numerics::float3 value)
{
	centerPoint = value;
}
