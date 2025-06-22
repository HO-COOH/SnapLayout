#include "pch.h"
#include "ThumbnailVisualContainerWindow.h"
#include "Interop.hpp"
#include "DirectXFactory.h"
#include "FreezableThumbnailVisual.h"
#include <winrt/Windows.Graphics.Capture.h>
#include <windows.ui.composition.interop.h>
#include "DebugHelper.hpp"
#include <dwmapi.h>
#include "OverviewWindow.xaml.h"

ThumbnailVisualContainerWindow::ThumbnailVisualContainerWindow() : BaseWindow{
		L"ThumbnailVisualContainer",
#if defined(_DEBUG) || defined(DEBUG) // in debug let the window show on taskbar
		WS_EX_OVERLAPPEDWINDOW | WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOPMOST/* | WS_EX_TOOLWINDOW*/,
#else
		WS_EX_OVERLAPPEDWINDOW | WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
#endif
		WS_POPUPWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1000,
		640
	}
{
	DirectXFactory::Init();
	auto interopCompositorFactory = winrt::get_activation_factory<winrt::Windows::UI::Composition::Compositor, IInteropCompositorFactoryPartner>();
	winrt::com_ptr<IInteropCompositorPartner> interopCompositor;
	winrt::check_hresult(interopCompositorFactory->CreateInteropCompositor(
		DirectXFactory::d2dDevice.get(),
		nullptr,
		IID_PPV_ARGS(interopCompositor.put())));


	auto dcompDevice = interopCompositor.as<IDCompositionDesktopDevice>().detach();
	winrt::com_ptr<IDCompositionTarget> dcompTarget;
	winrt::check_hresult(dcompDevice->CreateTargetForHwnd(m_hwnd.get(), true, dcompTarget.put()));

	compositor = interopCompositor.as<winrt::Windows::UI::Composition::Compositor>();
	root = compositor.CreateContainerVisual();
	dcompTarget.as<winrt::Windows::UI::Composition::CompositionTarget>().Root(root);
	dcompTarget.detach();

	compositionGraphicsDevice = winrt::Microsoft::Graphics::Canvas::UI::Composition::CanvasComposition::CreateCompositionGraphicsDevice(
		compositor,
		canvasDevice
	);

}

HWND ThumbnailVisualContainerWindow::GetHwnd() const noexcept
{
	return m_hwnd.get();
}

void ThumbnailVisualContainerWindow::SetVisual(HWND sourceHwnd, winrt::Windows::Foundation::Numerics::float2 animationCenter)
{
	Show();

	currentVisualHwnd = sourceHwnd;
	RECT windowPosition;
	winrt::check_bool(GetWindowRect(currentVisualHwnd, &windowPosition));

	winrt::check_bool(SetWindowPos(
		m_hwnd.get(),
		NULL,
		9999,
		9999,
		windowPosition.right - windowPosition.left,
		windowPosition.bottom - windowPosition.top,
		SWP_NOACTIVATE)
	);

	surface = compositionGraphicsDevice.CreateDrawingSurface(
		{ 
			static_cast<float>(windowPosition.right - windowPosition.left),
			static_cast<float>(windowPosition.bottom - windowPosition.top)
		},
		winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
		winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied
	);

	visual.emplace(sourceHwnd, m_hwnd.get(), compositor, surface, canvasDevice, root.Children());
	visual->CenterPoint({ animationCenter, 0.f });
}

void ThumbnailVisualContainerWindow::SetVisual(HWND sourceHwnd, POINT animationCenter)
{
	SetVisual(sourceHwnd, winrt::Windows::Foundation::Numerics::float2{
		static_cast<float>(animationCenter.x),
		static_cast<float>(animationCenter.y)
	});
}

void ThumbnailVisualContainerWindow::StartAnimation()
{
	Show();
	visual->Scale(winrt::Windows::Foundation::Numerics::float3::one());
	if (!shrinkAnimation)
	{
		shrinkAnimation = compositor.CreateVector3KeyFrameAnimation();
		shrinkAnimation.InsertKeyFrame(1.f, { 0.2f, 0.2f, 1.f });
		shrinkAnimation.Duration(duration);
	}
	visual->StartAnimation(L"Scale", shrinkAnimation);

	RECT windowPosition;
	winrt::check_bool(GetWindowRect(currentVisualHwnd, &windowPosition));
	winrt::check_bool(SetWindowPos(
		m_hwnd.get(),
		HWND_TOP,
		windowPosition.left,
		windowPosition.top,
		windowPosition.right - windowPosition.left,
		windowPosition.bottom - windowPosition.top,
		SWP_NOACTIVATE)
	);
}

ThumbnailVisualContainerWindow& ThumbnailVisualContainerWindow::Instance()
{
	static ThumbnailVisualContainerWindow s_instance;
	return s_instance;
}

void ThumbnailVisualContainerWindow::Hide(bool animation)
{
	if (!animation)
	{
		ShowWindow(m_hwnd.get(), SW_HIDE);
		return;
	}

	if (!visual)
		return;

	if (!restoreAnimation)
	{
		restoreAnimation = compositor.CreateVector3KeyFrameAnimation();
		restoreAnimation.InsertKeyFrame(1.f, winrt::Windows::Foundation::Numerics::float3::one());
		restoreAnimation.Duration(duration);
	}
	auto scopedBatch = compositor.CreateScopedBatch(winrt::Windows::UI::Composition::CompositionBatchTypes::Animation);
	scopedBatch.Completed([this](auto&&...) { 
		ShowWindow(m_hwnd.get(), SW_HIDE);
		ShowWindow(currentVisualHwnd, SW_SHOWNOACTIVATE);
		if (IsWindowVisible(winrt::SnapLayout::implementation::OverviewWindow::Instance))
		{
			SetActiveWindow(winrt::SnapLayout::implementation::OverviewWindow::Instance);
			SetForegroundWindow(winrt::SnapLayout::implementation::OverviewWindow::Instance);
		}
	});
	visual->StartAnimation(L"Scale", restoreAnimation);
	scopedBatch.End();
}

void ThumbnailVisualContainerWindow::Move(int x, int y)
{
	winrt::check_bool(SetWindowPos(
		m_hwnd.get(),
		HWND_TOP,
		x,
		y,
		0,
		0,
		SWP_NOSIZE | SWP_NOACTIVATE
	));
}

void ThumbnailVisualContainerWindow::Show()
{
	ShowWindow(m_hwnd.get(), SW_SHOWNOACTIVATE);
}
