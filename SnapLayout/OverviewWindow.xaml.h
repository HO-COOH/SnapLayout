#pragma once
#include "OverviewWindow.g.h"
#include "RoundedAcrylicVisual.h"
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include "INotifyWindowEvent.hpp"
#include <unordered_map>
#include "LayoutResult.h"
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include "OverviewData.h"

struct IDCompositionDesktopDevice;


namespace winrt::SnapLayout::implementation
{
    struct OverviewWindow : OverviewWindowT<OverviewWindow>
    {
        OverviewWindow();

        void RootGrid_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        /**
		 * @brief Show the overview window and place it at the specified position
         * 
         * @param position
		 * @param initWindows If true, the overview window will re-enumerate all windows the list of windows to display
         * @retval true if a window thumbnail is selected and we should continue to layout other empty positions
		 * @retval false if the overview window is dismissed by the user and we should abort the layout process
         */
        winrt::Windows::Foundation::IAsyncOperation<bool> ShowAndPlaceWindowAsync(LayoutResult position, bool initWindows, HWND excludeWindow);
        void Hide();

        void SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
		void WindowThumbnail_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        winrt::Windows::Foundation::Collections::IObservableVector<SnapLayout::WindowModel> Windows();
        void Window_Activated(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs const& args);
        static winrt::Microsoft::UI::Xaml::Visibility ConvertIconToVisibility(winrt::Microsoft::UI::Xaml::Media::ImageSource const& icon);

        static HWND Instance;
        static IDCompositionDesktopDevice* dcompDevice;
    private:
        winrt::Microsoft::UI::Content::ContentExternalBackdropLink m_backdropLink{ nullptr };
        winrt::Microsoft::UI::Composition::SystemBackdrops::DesktopAcrylicController m_controller{ nullptr };
        winrt::Microsoft::UI::Composition::ScalarKeyFrameAnimation m_opacityAnimation{ nullptr };
		OverviewData m_overviewData;
        RoundedAcrylicVisual m_acrylicVisual{ nullptr };
        HWND m_hwnd;
        LayoutResult m_windowPlacement{};
        bool m_isWindowSelected{};
        winrt::handle m_windowSelectedEvent;
    };
}

namespace winrt::SnapLayout::factory_implementation
{
    struct OverviewWindow : OverviewWindowT<OverviewWindow, implementation::OverviewWindow>
    {
    };
}
