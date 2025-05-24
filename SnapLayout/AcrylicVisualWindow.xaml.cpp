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
		auto compositor = visual.Compositor();

		m_backdropLink = winrt::Microsoft::UI::Content::ContentExternalBackdropLink::Create(compositor);
		m_backdropLink.ExternalBackdropBorderMode(winrt::Microsoft::UI::Composition::CompositionBorderMode::Soft);
		m_controller = {};
		m_controller.AddSystemBackdropTarget(m_backdropLink.as<winrt::Microsoft::UI::Composition::ICompositionSupportsSystemBackdrop>());
		m_controller.SetSystemBackdropConfiguration(m_configuration);
		m_controller.Kind(winrt::Microsoft::UI::Composition::SystemBackdrops::DesktopAcrylicKind::Thin);
		m_acrylicVisual = RoundedAcrylicVisual{ m_backdropLink.PlacementVisual(), compositor, CornerRadius };
		winrt::Microsoft::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(element, m_acrylicVisual);
	}

	void AcrylicVisualWindow::SetVisualPosition(LayoutResult finalLayoutPosition, UINT dpi)
	{
		winrt::check_hresult(SetWindowPos(m_hwnd, WindowDragEventListener::g_hwndTracked, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE));
		ShowWindow(m_hwnd, SW_SHOWNOACTIVATE);
		RECT draggedWindowPosition{};
		winrt::check_bool(GetWindowRect(WindowDragEventListener::g_hwndTracked, &draggedWindowPosition));

		float const unscaledWidth = UnscaleForDpi(draggedWindowPosition.right - draggedWindowPosition.left, dpi);
		float const unscaledHeight = UnscaleForDpi(draggedWindowPosition.bottom - draggedWindowPosition.top, dpi);
	
		m_acrylicVisual.StartSizeAnimation({ unscaledWidth, unscaledHeight }, { finalLayoutPosition.width, finalLayoutPosition.height });
		m_acrylicVisual.StartOffsetAnimation(
			{
				static_cast<float>(UnscaleForDpi(draggedWindowPosition.left, dpi)),
				static_cast<float>(UnscaleForDpi(draggedWindowPosition.top, dpi))
			}, 
			{ 
				finalLayoutPosition.x, 
				finalLayoutPosition.y 
			}
		);
	}

	void AcrylicVisualWindow::Hide()
	{
		m_acrylicVisual.Hide();
		AppWindow().Hide();
	}
}


