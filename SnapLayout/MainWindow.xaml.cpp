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

		
		m_appWindow = AppWindow();
		
		m_appWindow.Presenter().as<winrt::Microsoft::UI::Windowing::OverlappedPresenter>().IsAlwaysOnTop(true);
		m_appWindow.IsShownInSwitchers(false);

		SetWindowSubclass(g_instance,
			&subclassProc,
			0x100,
			reinterpret_cast<DWORD_PTR>(this));
		Instance = *this;
	}

	void MainWindow::Grid_PointerEntered(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e)
	{
		DebugLog(L"Pointer entered\n");
	}

	LRESULT MainWindow::subclassProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR uIdSubClass, DWORD_PTR dwRefData)
	{
		switch (msg)
		{
			case WM_MOUSEMOVE:
			{
				if (!WindowDragEventListener::HasWindowDragging())
					break;

				auto const draggedWindow = WindowDragEventListener::GetDraggedWindow();

				auto self = reinterpret_cast<MainWindow*>(dwRefData);
				auto const dpi = GetDpiForWindow(hwnd);
				POINT point{ GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
				POINT clientPoint = point;
				ScreenToClient(g_instance, &clientPoint);
				float const xPos = UnscaleForDpi(clientPoint.x, dpi);
				float const yPos = UnscaleForDpi(clientPoint.y, dpi);
				auto hitTest = winrt::Microsoft::UI::Xaml::Media::VisualTreeHelper::FindElementsInHostCoordinates(
					{ xPos, yPos },
					self->RootGrid()
				);

				//TODO: This move needs to be corrected with an offset
				if (self->thumbnailWindow)
				{
					auto draggedPointOffset = WindowDragEventListener::GetDraggedWindowPointOffset();
					self->thumbnailWindow->Move(point.x - draggedPointOffset.x, point.y - draggedPointOffset.y);
				}
				for (auto hitTestElement : hitTest)
				{
					if (auto button = hitTestElement.try_as<winrt::Microsoft::UI::Xaml::Controls::Button>())
					{
						//put button to hover state, show window thumbnail visual



						if (button == self->m_previousButton)
						{

						}
						else
						{
							//hovered on a new button
							if (self->m_previousButton)
								winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(self->m_previousButton, L"Normal", true);
							winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(button, L"PointerOver", true);
							self->m_previousButton = button;
							self->m_previousButtonWindowPlacement = LayoutImpl(GetButtonLayoutResult(button,
								button.Parent().as<winrt::Microsoft::UI::Xaml::Controls::Grid>()),
								MonitorFromWindow(g_instance, MONITOR_DEFAULTTONEAREST),
								draggedWindow
							);
							self->thumbnailWindow = &ThumbnailVisualContainerWindow::Instance();
							WindowDragEventListener::HideDraggedWindow(point, dpi);
						}

						break;
					}
				}
				break;
			}

			case WM_MOUSELEAVE:
			{
			
				//ShowWindow(WindowDragEventListener::g_hwndTracked, SW_SHOW);
				auto self = reinterpret_cast<MainWindow*>(dwRefData);
				if (self->m_previousButton)
				{
					winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(self->m_previousButton, L"Normal", true);
					self->m_previousButton = nullptr;
				}
				winrt::get_self<winrt::SnapLayout::implementation::AcrylicVisualWindow>(winrt::SnapLayout::implementation::AcrylicVisualWindow::Instance)->Hide();
				//ThumbnailVisualContainerWindow::Instance().Hide();
				winrt::SnapLayout::implementation::AcrylicVisualWindow::Instance.AppWindow().Hide();
				if (self->thumbnailWindow)
				{
					self->thumbnailWindow->Hide();
					self->thumbnailWindow = nullptr;
				}
				if (WindowDragEventListener::HasWindowDragging())
					ShowWindow(WindowDragEventListener::GetDraggedWindow(), SW_SHOW);
				break;
			}

			case WM_LBUTTONUP:
			{
				if(!WindowDragEventListener::HasWindowDragging())
					break;

				auto self = reinterpret_cast<MainWindow*>(dwRefData);
				self->OnDismiss();

				auto const draggedWindow = WindowDragEventListener::GetDraggedWindow();
				if (!draggedWindow)
					break;

	
				if (self->m_previousButton)
				{
					winrt::check_bool(SetWindowPos(draggedWindow, nullptr,
						self->m_previousButtonWindowPlacement.x,
						self->m_previousButtonWindowPlacement.y,
						self->m_previousButtonWindowPlacement.width,
						self->m_previousButtonWindowPlacement.height,
						0
					));
				}
				self->OnDismiss();
				break;
			}
		}
		return DefSubclassProc(hwnd, msg, wparam, lparam);
	}

	LayoutResult MainWindow::GetButtonLayoutResult(winrt::Microsoft::UI::Xaml::Controls::Button const& button, winrt::Microsoft::UI::Xaml::Controls::Grid const& parentGrid)
	{
		/*Instead of calculating rowTotal, buttonRow in two pass, we can hand write one for loop, better performance*/

		auto const buttonRow = winrt::Microsoft::UI::Xaml::Controls::Grid::GetRow(button);
		auto const buttonRowSpan = winrt::Microsoft::UI::Xaml::Controls::Grid::GetRowSpan(button);

		double rowBeforeButton{}, rowTotal{}, buttonRowTotal{};
		for (int i = 0; auto row : parentGrid.RowDefinitions())
		{
			auto const current = row.Height().Value;
			rowTotal += current;
			if (i < buttonRow)
				rowBeforeButton += current;
			else if (i >= buttonRow && i < buttonRow + buttonRowSpan)
				buttonRowTotal += current;
			++i;
		}
		if (rowTotal == 0)
		{
			//means we only have 1 row, and the button is occupying the whole row
			rowTotal = 1;
			buttonRowTotal = 1;
		}

		auto const buttonCol = winrt::Microsoft::UI::Xaml::Controls::Grid::GetColumn(button);
		auto const buttonColSpan = winrt::Microsoft::UI::Xaml::Controls::Grid::GetColumnSpan(button);
		double colBeforeButton{}, colTotal{}, buttonColTotal{};
		for (int i = 0; auto col : parentGrid.ColumnDefinitions())
		{
			auto const current = col.Width().Value;
			colTotal += current;
			if (i < buttonCol)
				colBeforeButton += current;
			else if (i >= buttonCol && i < buttonCol + buttonColSpan)
				buttonColTotal += current;
			++i;
		}

		if (colTotal == 0)
		{
			colTotal = 1;
			buttonColTotal = 1;
		}

		return LayoutResult{ 
			.x = static_cast<float>(colBeforeButton / colTotal), 
			.y = static_cast<float>(rowBeforeButton / rowTotal),
			.width = static_cast<float>(buttonColTotal / colTotal),
			.height = static_cast<float>(buttonRowTotal / rowTotal)
		};
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
		m_appWindow.Presenter().as<winrt::Microsoft::UI::Windowing::OverlappedPresenter>().IsAlwaysOnTop(true);
		GridAppearAnimation().Begin();
	}

	void MainWindow::OnDismiss()
	{
		m_shouldHideWindow = true;
		GridAppearAnimation().Stop();
		if (m_hasExitCompleted)
		{
			GridExitAnimation().Begin();
			m_hasExitCompleted = false;
		}
	}

	void MainWindow::moveToMonitor(HMONITOR monitor)
	{
		MONITORINFO info{ .cbSize = sizeof(info) };
		winrt::check_bool(GetMonitorInfoW(monitor, &info));
		
		UINT dpi, dpiY;
		winrt::check_hresult(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpi, &dpiY));

		winrt::Windows::Graphics::SizeInt32 windowSize{ ScaleForDpi<int>(1000, dpi), ScaleForDpi<int>(133, dpi) };
		
		//bug: https://github.com/microsoft/microsoft-ui-xaml/issues/10498
		//m_appWindow.MoveAndResize(winrt::Windows::Graphics::RectInt32{ 
		//	.X = info.rcWork.left + (info.rcWork.right - info.rcWork.left - windowSize.Width) / 2, 
		//	.Y = info.rcWork.top, 
		//	.Width = windowSize.Width, 
		//	.Height = windowSize.Height 
		//});

		winrt::check_bool(SetWindowPos(
			g_instance,
			nullptr,
			info.rcWork.left + (info.rcWork.right - info.rcWork.left - windowSize.Width) / 2,
			info.rcWork.top,
			windowSize.Width,
			windowSize.Height,
			SWP_NOACTIVATE | SWP_NOZORDER
		));
	}

	void MainWindow::OnGridExitAnimationCompleted(
		winrt::Windows::Foundation::IInspectable const& sender,
		winrt::Windows::Foundation::IInspectable const& arg
	)
	{
		m_hasExitCompleted = true;
		if (m_shouldHideWindow)
			AppWindow().Hide();
	}
}

