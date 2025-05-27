#pragma once

#include "MainWindow.g.h"
#include "LayoutResult.h"

class ThumbnailVisualContainerWindow;

namespace winrt::SnapLayout::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
        void Grid_PointerEntered(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);

        static HWND g_instance;

        static LRESULT CALLBACK subclassProc(
            HWND hwnd,
            UINT msg,
            WPARAM wparam,
            LPARAM lparam,
            UINT_PTR uIdSubClass,
            DWORD_PTR dwRefData
        );

        static LayoutResult GetButtonLayoutResult(
            winrt::Microsoft::UI::Xaml::Controls::Button const& button,
            winrt::Microsoft::UI::Xaml::Controls::Grid const& parentGrid
        );

        //std::unordered_map<winrt::Microsoft::UI::Xaml::Controls::Button, decltype(&LayoutWindowByButton)> layoutFunctions;
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
        ThumbnailVisualContainerWindow* thumbnailWindow{ nullptr };
        winrt::Microsoft::UI::Windowing::AppWindow m_appWindow{ nullptr };
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
