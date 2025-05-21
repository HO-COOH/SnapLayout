#include "pch.h"
#include "AcrylicVisualWindow.xaml.h"
#if __has_include("AcrylicVisualWindow.g.cpp")
#include "AcrylicVisualWindow.g.cpp"
#endif
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include "WindowDragEventListener.h"
#include "DpiUtils.hpp"
#include "WindowUtils.hpp"
#include <winrt/Microsoft.UI.Windowing.h>

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::SnapLayout::implementation
{
	winrt::SnapLayout::AcrylicVisualWindow AcrylicVisualWindow::Instance{ nullptr };

	winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration AcrylicVisualWindow::m_configuration =
		[]()
		{
			winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration configuration;
			configuration.IsInputActive(true);
			//configuration.Theme(winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropTheme::Light);
			return configuration;
		}();

	AcrylicVisualWindow::AcrylicVisualWindow()
	{
		Instance = *this;
		AppWindow().IsShownInSwitchers(false);
		m_hwnd = GetHwnd(*this);
	}

	void AcrylicVisualWindow::RootGrid_Loaded(
		winrt::Windows::Foundation::IInspectable const& sender, 
		winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		auto element = sender.as<winrt::Microsoft::UI::Xaml::UIElement>();
		auto visual = winrt::Microsoft::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(element);
		m_compositor = visual.Compositor();

		m_backdropLink = winrt::Microsoft::UI::Content::ContentExternalBackdropLink::Create(m_compositor);
		m_backdropLink.ExternalBackdropBorderMode(winrt::Microsoft::UI::Composition::CompositionBorderMode::Soft);
		m_controller = {};
		m_controller.AddSystemBackdropTarget(m_backdropLink.as<winrt::Microsoft::UI::Composition::ICompositionSupportsSystemBackdrop>());
		m_controller.SetSystemBackdropConfiguration(m_configuration);
		m_controller.Kind(winrt::Microsoft::UI::Composition::SystemBackdrops::DesktopAcrylicKind::Thin);

		m_placementVisual = m_backdropLink.PlacementVisual();
		m_placementVisual.BorderMode(winrt::Microsoft::UI::Composition::CompositionBorderMode::Soft);

		//auto size = element.ActualSize();

		//auto animation = compositor.CreateVector2KeyFrameAnimation();
		//animation.InsertKeyFrame(1.f, size);
		//animation.Duration(duration);
		//m_placementVisual.StartAnimation(L"Size", animation);

		m_clip = m_compositor.CreateRectangleClip(
			0,
			0,
			10.f,
			10.f,
			{ CornerRadius, CornerRadius },
			{ CornerRadius, CornerRadius },
			{ CornerRadius, CornerRadius },
			{ CornerRadius, CornerRadius }
		);

		//auto rightAnimation = compositor.CreateScalarKeyFrameAnimation();
		//rightAnimation.Duration(duration);
		//rightAnimation.InsertKeyFrame(1.f, size.x);
		//clip.StartAnimation(L"Right", rightAnimation);

		//auto bottomAnimation = compositor.CreateScalarKeyFrameAnimation();
		//bottomAnimation.Duration(duration);
		//bottomAnimation.InsertKeyFrame(1.f, size.y);
		//clip.StartAnimation(L"Bottom", bottomAnimation);

		m_placementVisual.Clip(m_clip);

		winrt::Microsoft::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(element, m_placementVisual);
		//winrt::Microsoft::UI::Xaml::Hosting::ElementCompositionPreview::SetIsTranslationEnabled(element, true);
	}

	void AcrylicVisualWindow::SetVisualPosition(LayoutResult position, UINT dpi)
	{
		winrt::check_hresult(SetWindowPos(m_hwnd, WindowDragEventListener::g_hwndTracked, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE));
		ShowWindow(m_hwnd, SW_SHOWNOACTIVATE);
		RECT draggedWindowPosition{};
		winrt::check_bool(GetWindowRect(WindowDragEventListener::g_hwndTracked, &draggedWindowPosition));

		float const unscaledWidth = UnscaleForDpi(draggedWindowPosition.right - draggedWindowPosition.left, dpi);
		float const unscaledHeight = UnscaleForDpi(draggedWindowPosition.bottom - draggedWindowPosition.top, dpi);
		auto placementVisualSizeAnimation = m_compositor.CreateVector2KeyFrameAnimation();
		placementVisualSizeAnimation.InsertKeyFrame(0.f,
			{
				unscaledWidth,
				unscaledHeight
			});
		placementVisualSizeAnimation.InsertKeyFrame(1.f,
			{
				position.width,
				position.height
			});

		auto placementVisualOffsetAnimation = m_compositor.CreateVector2KeyFrameAnimation();
		placementVisualOffsetAnimation.InsertKeyFrame(0.f, {
			static_cast<float>(UnscaleForDpi(draggedWindowPosition.left, dpi)),
			static_cast<float>(UnscaleForDpi(draggedWindowPosition.top, dpi))
			});
		placementVisualOffsetAnimation.InsertKeyFrame(1.f, {
			position.x,
			position.y
			});

		auto clipRightAnimation = m_compositor.CreateScalarKeyFrameAnimation();
		clipRightAnimation.InsertKeyFrame(0.f, unscaledWidth);
		clipRightAnimation.InsertKeyFrame(1.f, position.width);

		auto clipBottomAnimation = m_compositor.CreateScalarKeyFrameAnimation();
		clipBottomAnimation.InsertKeyFrame(0.f, unscaledHeight);
		clipBottomAnimation.InsertKeyFrame(1.f, position.height);

		m_placementVisual.StartAnimation(L"Size", placementVisualSizeAnimation);
		m_placementVisual.StartAnimation(L"Offset.XY", placementVisualOffsetAnimation);
		m_clip.StartAnimation(L"Right", clipRightAnimation);
		m_clip.StartAnimation(L"Bottom", clipBottomAnimation);


	}

	void AcrylicVisualWindow::Hide()
	{
		m_placementVisual.Size({ 1.f, 1.f });
		AppWindow().Hide();
	}
}


