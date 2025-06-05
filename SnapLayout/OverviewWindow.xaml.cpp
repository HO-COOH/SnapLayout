#include "pch.h"
#include "OverviewWindow.xaml.h"
#if __has_include("OverviewWindow.g.cpp")
#include "OverviewWindow.g.cpp"
#endif
#include "AcrylicVisualWindow.xaml.h"
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include "WindowDragEventListener.h"
#include "WindowUtils.hpp"

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::SnapLayout::implementation
{
	OverviewWindow::OverviewWindow()
	{
		m_hwnd = GetHwnd(*this);
	}

	void OverviewWindow::RootGrid_Loaded(
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
		m_controller.SetSystemBackdropConfiguration(AcrylicVisualWindow::m_configuration);
		m_controller.Kind(winrt::Microsoft::UI::Composition::SystemBackdrops::DesktopAcrylicKind::Thin);
		m_acrylicVisual = RoundedAcrylicVisual{ m_backdropLink.PlacementVisual(), compositor, AcrylicVisualWindow::CornerRadius };
		winrt::Microsoft::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(element, m_acrylicVisual);
	}

	winrt::Windows::Foundation::Collections::IObservableVector<SnapLayout::WindowModel> OverviewWindow::Windows()
	{
		return m_windows;
	}

	void OverviewWindow::OnWindowCreated(HWND createdWindow)
	{
		SnapLayout::WindowModel model{ reinterpret_cast<uint64_t>(createdWindow) };
		m_windows.Append(model);
		m_windowRef[createdWindow] = winrt::make_weak(model);
	}

	void OverviewWindow::OnWindowDestroyed(HWND destroyedWindow)
	{
		if (auto iter = m_windowRef.find(destroyedWindow); iter != m_windowRef.end())
		{
			uint32_t index{};
			m_windows.IndexOf(iter->second.get(), index);
			m_windows.RemoveAt(index);
			m_windowRef.erase(destroyedWindow);
		}
	}

	void OverviewWindow::Show(LayoutResult layout)
	{
		WindowDragEventListener::SubscribeWindowEvent(this);
		ShowWindow(m_hwnd, SW_SHOWNOACTIVATE);
		SetWindowPos(m_hwnd, nullptr, layout.x, layout.y, layout.width, layout.height, SWP_NOZORDER | SWP_NOACTIVATE);
		m_acrylicVisual.StartSizeAnimation({ 10.f, 10.f }, { layout.width, layout.height });
	}

	void OverviewWindow::Hide()
	{
		WindowDragEventListener::UnsubscribeWindowEvent(this);
	}
}
