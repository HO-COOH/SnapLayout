#pragma once

#include "MainWindow.g.h"
#include "OverviewWindow.xaml.h"
#include "LayoutResult.h"
#include "ButtonLayoutCache.h"

class ThumbnailVisualContainerWindow;

namespace winrt::SnapLayout::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
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

        winrt::Microsoft::UI::Xaml::Controls::Button m_previousButton{ nullptr };

        void OnShow(HMONITOR draggedWindowMonitor);
        void OnDismiss();


        void OnGridExitAnimationCompleted(
            winrt::Windows::Foundation::IInspectable const& sender,
            winrt::Windows::Foundation::IInspectable const& arg
		);

        LayoutResult m_previousButtonWindowPlacement;
        static MainWindow* GetInstance();
    private:
        void moveToMonitor(HMONITOR monitor);
        winrt::fire_and_forget layoutOtherWindows();
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
