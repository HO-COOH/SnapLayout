#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include <winrt/Microsoft.UI.Windowing.h>
#include "WindowUtils.hpp"
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <CommCtrl.h>
#include <winrt/Windows.UI.h>
#include <windowsx.h>
#include "LayoutFunctions.h"
#include "DpiUtils.hpp"
#include "WindowDragEventListener.h"
#include <numeric>
#include <dwmapi.h>
#include "AcrylicVisualWindow.xaml.h"
#include "ThumbnailVisualContainerWindow.h"
#include "DebugHelper.hpp"
#include <ShellScalingApi.h>
#include "OverviewWindowFilter.h"
#include "MonitorWrapper.h"
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "Comctl32.lib")


// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::SnapLayout::implementation
{
	HWND MainWindow::g_instance;

	winrt::SnapLayout::MainWindow MainWindow::Instance{ nullptr };

	MainWindow::MainWindow()
	{
		g_instance = GetHwnd(*this);
		AddToFilter(g_instance);
		
		m_appWindow = AppWindow();
		m_appWindow.Presenter().as<winrt::Microsoft::UI::Windowing::OverlappedPresenter>().IsAlwaysOnTop(true);
		m_appWindow.IsShownInSwitchers(false);

		SetWindowSubclass(g_instance,
			&subclassProc,
			0x100,
			reinterpret_cast<DWORD_PTR>(this));
		Instance = *this;
	}

	void MainWindow::onMouseMove(HWND hwnd, POINT point)
	{
		if (!WindowDragEventListener::HasWindowDragging())
			return;

		//TODO: This move needs to be corrected with an offset
		if (thumbnailWindow)
		{
			auto draggedPointOffset = WindowDragEventListener::GetDraggedWindowPointOffset();
			thumbnailWindow->Move(point.x - draggedPointOffset.x, point.y - draggedPointOffset.y);
		}

		auto const draggedWindow = WindowDragEventListener::GetDraggedWindow();
		auto const dpi = GetDpiForWindow(hwnd);
		POINT clientPoint = point;
		ScreenToClient(g_instance, &clientPoint);
		for (
			auto hitTest = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::FindElementsInHostCoordinates(
			UnscalePointForDpi<>(clientPoint, dpi),
			RootGrid()
			); auto hitTestElement : hitTest)
		{
			if (auto button = hitTestElement.try_as<winrt::Microsoft::UI::Xaml::Controls::Button>())
			{
				//put button to hover state, show window thumbnail visual

				if (!m_previousButton)
				{
					WindowDragEventListener::HideDraggedWindow(point, dpi);
				}

				if (m_previousButton != button)
				{
					//hovered on a new button
					m_previousButton = button;
					m_previousButtonWindowPlacement = LayoutImpl(
						m_buttonLayoutCache.GetLayout(button),
						Monitor::FromWindow(g_instance),
						draggedWindow
					);
					thumbnailWindow = &ThumbnailVisualContainerWindow::Instance();
				}

				return;
			}
		}
	}

	void MainWindow::onMouseLeave()
	{
		//ShowWindow(WindowDragEventListener::g_hwndTracked, SW_SHOW);
		m_previousButton = nullptr;
		winrt::get_self<winrt::SnapLayout::implementation::AcrylicVisualWindow>(winrt::SnapLayout::implementation::AcrylicVisualWindow::Instance)->Hide();
		//ThumbnailVisualContainerWindow::Instance().Hide();
		if (thumbnailWindow)
		{
			thumbnailWindow->Hide();
			thumbnailWindow = nullptr;
		}
	}

	void MainWindow::onLButtonUp()
	{
		if (!WindowDragEventListener::HasWindowDragging())
			return;

		OnDismiss();

		auto const draggedWindow = WindowDragEventListener::GetDraggedWindow();
		if (!draggedWindow)
			return;


		if (m_previousButton)
		{
			winrt::check_bool(SetWindowPos(
				draggedWindow,
				nullptr,
				static_cast<int>(m_previousButtonWindowPlacement.x),
				static_cast<int>(m_previousButtonWindowPlacement.y),
				static_cast<int>(m_previousButtonWindowPlacement.width),
				static_cast<int>(m_previousButtonWindowPlacement.height),
				SWP_NOACTIVATE
			));
			if (thumbnailWindow)
			{
				thumbnailWindow->Hide();
				thumbnailWindow = nullptr;
			}

			layoutOtherWindows(draggedWindow);
			m_previousButton = nullptr;
		}
	}

	LRESULT MainWindow::subclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, [[maybe_unused]] UINT_PTR uIdSubClass, DWORD_PTR dwRefData)
	{
		switch (msg)
		{
			case WM_MOUSEMOVE:
				reinterpret_cast<MainWindow*>(dwRefData)->onMouseMove(hwnd, { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) });
				break;
			case WM_MOUSELEAVE:
				reinterpret_cast<MainWindow*>(dwRefData)->onMouseLeave();
				break;
			case WM_LBUTTONUP:
				reinterpret_cast<MainWindow*>(dwRefData)->onLButtonUp();
				break;
			default:
				break;
		}
		return DefSubclassProc(hwnd, msg, wparam, lparam);
	}

	winrt::fire_and_forget MainWindow::layoutOtherWindows(HWND excludeWindow)
	{
		auto previousButtonCopy = m_previousButton.Get();
		auto const monitor = MonitorFromWindow(g_instance, MONITOR_DEFAULTTONEAREST);
		for (bool initWindows = true; auto parentGridChild : previousButtonCopy.Parent().as<winrt::Microsoft::UI::Xaml::Controls::Grid>().Children())
		{
			if (auto button = parentGridChild.as<winrt::Microsoft::UI::Xaml::Controls::Button>(); button != previousButtonCopy)
			{
				LayoutResult overviewWindowPlacement = m_buttonLayoutCache.GetLayout(button);
				ConvertLayoutToMonitorWindowPlacement(overviewWindowPlacement, monitor);
				if (!(co_await m_overviewWindowImpl->ShowAndPlaceWindowAsync(overviewWindowPlacement, initWindows, excludeWindow)))
					break;
				initWindows = false;
			}
		}
		co_return;
	}

	MainWindow* MainWindow::GetInstance()
	{
		return winrt::get_self<MainWindow>(Instance);
	}

	void MainWindow::OnShow(HMONITOR draggedWindowMonitor)
	{
		if (!m_hasExitCompleted)
		{
			GridExitAnimation().Stop();
			m_shouldHideWindow = false;
		}
		m_hasExitCompleted = true;
		moveToMonitor(draggedWindowMonitor);
		ShowWindow(g_instance, SW_SHOWNOACTIVATE);
		WindowDragEventListener::SubscribeWindowMonitorChangeEvent(this);
		m_appWindow.Presenter().as<winrt::Microsoft::UI::Windowing::OverlappedPresenter>().IsAlwaysOnTop(true);
		GridAppearAnimation().Begin();
	}

	void MainWindow::OnDismiss()
	{
		WindowDragEventListener::UnsubscribeWindowMonitorChangeEvent();
		m_shouldHideWindow = true;
		GridAppearAnimation().Stop();
		if (m_hasExitCompleted)
		{
			GridExitAnimation().Begin();
			m_hasExitCompleted = false;
		}
	}

	void MainWindow::moveToMonitor(Monitor const& monitor)
	{
		auto const info = monitor.GetInfo();
		auto const dpi = monitor.GetDpi();

		winrt::Windows::Graphics::SizeInt32 windowSize{ ScaleForDpi<int>(1000, dpi), ScaleForDpi<int>(133, dpi) };
		
		//bug: https://github.com/microsoft/microsoft-ui-xaml/issues/10498
		//m_appWindow.MoveAndResize(winrt::Windows::Graphics::RectInt32{ 
		//	.X = info.rcWork.left + (info.rcWork.right - info.rcWork.left - windowSize.Width) / 2, 
		//	.Y = info.rcWork.top, 
		//	.Width = windowSize.Width, 
		//	.Height = windowSize.Height 
		//});

		//We move the window first to the target monitor, then resize it, otherwise the size will be scaled to an incorrect value by system
		winrt::check_bool(SetWindowPos(g_instance,
			thumbnailWindow ? thumbnailWindow->GetHwnd() : nullptr,
			info.rcWork.left + (info.rcWork.right - info.rcWork.left - windowSize.Width) / 2,
			info.rcWork.top,
			0, 0,
			SWP_NOSIZE | SWP_NOACTIVATE));

		winrt::check_bool(SetWindowPos(
			g_instance,
			thumbnailWindow? thumbnailWindow->GetHwnd() : nullptr,
			info.rcWork.left + (info.rcWork.right - info.rcWork.left - windowSize.Width) / 2,
			info.rcWork.top,
			windowSize.Width,
			windowSize.Height,
			SWP_NOACTIVATE
		));
		DebugLog(L"MainWindow W: {}, H: {}\n", windowSize.Width, windowSize.Height);
	}

	void MainWindow::OnGridExitAnimationCompleted(
		winrt::Windows::Foundation::IInspectable const&,
		winrt::Windows::Foundation::IInspectable const&
	)
	{
		m_hasExitCompleted = true;
		if (m_shouldHideWindow)
			AppWindow().Hide();
	}

	void MainWindow::WindowMonitorChanged(HMONITOR monitor)
	{
		moveToMonitor(monitor);
	}
}

