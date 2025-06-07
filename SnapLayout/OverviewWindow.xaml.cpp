#include "pch.h"
#include "OverviewWindow.xaml.h"
#if __has_include("OverviewWindow.g.cpp")
#include "OverviewWindow.g.cpp"
#endif
#include "AcrylicVisualWindow.xaml.h"
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include "WindowDragEventListener.h"
#include "WindowUtils.hpp"
#include "Interop.hpp"
#include "DirectXFactory.h"
#include "OverviewWindowFilter.h"

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::SnapLayout::implementation
{
	HWND OverviewWindow::Instance;
	IDCompositionDesktopDevice* OverviewWindow::dcompDevice;

	OverviewWindow::OverviewWindow()
	{
		m_hwnd = GetHwnd(*this);

		Instance = m_hwnd;
		AddToFilter(m_hwnd);

		auto interopCompositorFactory = winrt::get_activation_factory<winrt::Windows::UI::Composition::Compositor, IInteropCompositorFactoryPartner>();
		winrt::com_ptr<IInteropCompositorPartner> interopCompositor;
		winrt::check_hresult(interopCompositorFactory->CreateInteropCompositor(
			DirectXFactory::d2dDevice.get(),
			nullptr,
			IID_PPV_ARGS(interopCompositor.put())));


		 dcompDevice = interopCompositor.as<IDCompositionDesktopDevice>().detach();
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

	void OverviewWindow::SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
	{
		auto control = sender.as<winrt::SnapLayout::ThumbnailVisualControl>();
		auto width = control.ActualWidth();
		control.Parent().as<winrt::Microsoft::UI::Xaml::Controls::Grid>().MaxWidth(width);
	}

	winrt::Windows::Foundation::Collections::IObservableVector<SnapLayout::WindowModel> OverviewWindow::Windows()
	{
		return m_windows;
	}

	void OverviewWindow::OnWindowCreated(HWND createdWindow)
	{
		if (IsInFilter(createdWindow))
			return;

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

	void OverviewWindow::ShowAndPlaceWindowAsync(LayoutResult layout)
	{
		initWindows();
		WindowDragEventListener::SubscribeWindowEvent(this);
		Activate();
		SetWindowPos(m_hwnd, nullptr, layout.x, layout.y, layout.width, layout.height, SWP_NOZORDER | SWP_NOACTIVATE);
		m_acrylicVisual.StartSizeAnimation({ 10.f, 10.f }, { layout.width, layout.height });
	}

	void OverviewWindow::Hide()
	{
		WindowDragEventListener::UnsubscribeWindowEvent(this);
		ShowWindow(m_hwnd, SW_HIDE);
	}

	void OverviewWindow::initWindows()
	{
		m_windows.Clear();
		m_windowRef.clear();

		static DWORD explorerPid = 0;
		HWND hShellWnd = FindWindow(L"Shell_TrayWnd", nullptr);
		GetWindowThreadProcessId(hShellWnd, &explorerPid);

		EnumWindows(
			+[](HWND hwnd, LPARAM lparam) -> BOOL
			{
				if (IsWindowResizable(hwnd, true) && !IsInFilter(hwnd))
				{
					wchar_t className[MAX_PATH]{};
					GetClassName(hwnd, className, std::size(className));
					DWORD pid;
					GetWindowThreadProcessId(hwnd, &pid);

					if (std::wstring_view{ className } == L"ApplicationFrameWindow" && pid == explorerPid)
						return true;

					reinterpret_cast<OverviewWindow*>(lparam)->OnWindowCreated(hwnd);
				}
				return true;
			},
			reinterpret_cast<LPARAM>(this)
		);
	}
}
