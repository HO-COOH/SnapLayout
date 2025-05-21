#include "pch.h"
#include "ThumbnailVisualContainerWindow.h"
#include "Interop.hpp"
#include "DirectXFactory.h"
#include "ThumbnailVisual.h"

ThumbnailVisualContainerWindow::ThumbnailVisualContainerWindow() : BaseWindow{
		L"ThumbnailVisualContainer",
		//TODO: add WS_EX_TOOLWINDOW when in release
		WS_EX_OVERLAPPEDWINDOW | WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOPMOST/* | WS_EX_TOOLWINDOW*/,
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
}

void ThumbnailVisualContainerWindow::SetVisual(HWND sourceHwnd, RECT windowPosition)
{
	ThumbnailVisual visual = { sourceHwnd, m_hwnd.get(),
		compositor.as<IDCompositionDesktopDevice>().get()
	};
	auto animation = compositor.CreateVector3KeyFrameAnimation();
	animation.InsertKeyFrame(1.f, { 0.2f, 0.2f, 1.f });
	animation.Duration(std::chrono::seconds{ 5 });
	visual.StartAnimation(L"Scale", animation);
	auto children = root.Children();
	children.RemoveAll();
	children.InsertAtTop(visual);
	winrt::check_bool(SetWindowPos(
		m_hwnd.get(), 
		nullptr, 
		windowPosition.left, 
		windowPosition.top, 
		windowPosition.right - windowPosition.left, 
		windowPosition.bottom - windowPosition.top, 
		SWP_NOZORDER)
	);
}

ThumbnailVisualContainerWindow& ThumbnailVisualContainerWindow::Instance()
{
	static ThumbnailVisualContainerWindow s_instance;
	return s_instance;
}

void ThumbnailVisualContainerWindow::Hide()
{
	ShowWindow(m_hwnd.get(), SW_HIDE);
}