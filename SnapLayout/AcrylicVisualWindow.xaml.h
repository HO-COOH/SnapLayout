#pragma once

#include "AcrylicVisualWindow.g.h"
#include "LayoutResult.h"
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include "RoundedAcrylicVisual.h"

namespace winrt::SnapLayout::implementation
{
    struct AcrylicVisualWindow : AcrylicVisualWindowT<AcrylicVisualWindow>
    {
        static winrt::SnapLayout::AcrylicVisualWindow Instance;
        AcrylicVisualWindow();
        void RootGrid_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        winrt::Microsoft::UI::Content::ContentExternalBackdropLink m_backdropLink{ nullptr };
        winrt::Microsoft::UI::Composition::SystemBackdrops::DesktopAcrylicController m_controller{ nullptr };
		RoundedAcrylicVisual m_acrylicVisual{ nullptr };

        /**
         * play an animation from the current dragging window position to the final layout position
         * 
         * @param position The final position of the visual
         * @param draggedWindow The window being dragged, this is needed for putting this visual window behind that dragged window, 
         *  and getting the starting position of the animation
         * @param dpi The dpi of the monitor of the dragged window
         */
        void SetVisualPosition(LayoutResult position, HWND draggedWindow, UINT dpi, RECT rcWork);
        static winrt::Microsoft::UI::Composition::SystemBackdrops::SystemBackdropConfiguration m_configuration;

        void Hide();
        constexpr static auto CornerRadius = 8.f;
        HWND m_hwnd;
    };
}

namespace winrt::SnapLayout::factory_implementation
{
    struct AcrylicVisualWindow : AcrylicVisualWindowT<AcrylicVisualWindow, implementation::AcrylicVisualWindow>
    {
    };
}
