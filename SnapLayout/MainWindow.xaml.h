#pragma once

#include "MainWindow.g.h"
#include "OverviewWindow.xaml.h"
#include "LayoutResult.h"
#include "ButtonLayoutCache.h"
#include "INotifyWindowMonitorChange.hpp"
#include "LayoutButtonHolder.h"

class ThumbnailVisualContainerWindow;
class Monitor;

namespace winrt::SnapLayout::implementation
{
    struct MainWindow : MainWindowT<MainWindow>, INotifyWindowMonitorChange
    {
        MainWindow();

        static HWND g_instance;

        static LRESULT CALLBACK subclassProc(
            HWND hwnd,
            UINT msg,
            WPARAM wparam,
            LPARAM lparam,
            UINT_PTR uIdSubClass,
            DWORD_PTR dwRefData
        );

        void onMouseMove(HWND self, POINT p);
        void onMouseLeave();
        void onLButtonUp();

        LayoutButtonHolder m_previousButton;

        void OnShow(HMONITOR draggedWindowMonitor);
        void OnDismiss();


        void OnGridExitAnimationCompleted(
            winrt::Windows::Foundation::IInspectable const& sender,
            winrt::Windows::Foundation::IInspectable const& arg
		);

        void WindowMonitorChanged(HMONITOR monitor) override;

        LayoutResult m_previousButtonWindowPlacement;
        static MainWindow* GetInstance();
    private:
        void moveToMonitor(Monitor const& monitor);
        winrt::fire_and_forget layoutOtherWindows(HWND excludeWindow);
        ThumbnailVisualContainerWindow* thumbnailWindow{ nullptr };
        winrt::SnapLayout::OverviewWindow m_overviewWindow;
        OverviewWindow* m_overviewWindowImpl = winrt::get_self<implementation::OverviewWindow>(m_overviewWindow);
        winrt::Microsoft::UI::Windowing::AppWindow m_appWindow{ nullptr };
        ButtonLayoutCache m_buttonLayoutCache;

        bool m_hasExitCompleted = true;
        bool m_shouldHideWindow = true;
        static winrt::SnapLayout::MainWindow Instance;
    };
}

namespace winrt::SnapLayout::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
