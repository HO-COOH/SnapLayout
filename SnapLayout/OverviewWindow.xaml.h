#pragma once
#include "OverviewWindow.g.h"
#include "RoundedAcrylicVisual.h"
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include "INotifyWindowEvent.hpp"
#include <unordered_map>
#include "LayoutResult.h"

struct IDCompositionDesktopDevice;

namespace winrt::SnapLayout::implementation
{
    struct OverviewWindow : OverviewWindowT<OverviewWindow>, INotifyWindowEvent
    {
        OverviewWindow();

        void RootGrid_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        void OnWindowCreated(HWND createdWindow) override;
        void OnWindowDestroyed(HWND destroyedWindow) override;

        void ShowAndPlaceWindowAsync(LayoutResult position);
        void Hide();

        void SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);

        winrt::Windows::Foundation::Collections::IObservableVector<SnapLayout::WindowModel> Windows();

        static HWND Instance;
        static IDCompositionDesktopDevice* dcompDevice;
    private:
        winrt::Microsoft::UI::Content::ContentExternalBackdropLink m_backdropLink{ nullptr };
        winrt::Microsoft::UI::Composition::SystemBackdrops::DesktopAcrylicController m_controller{ nullptr };
        RoundedAcrylicVisual m_acrylicVisual{ nullptr };
        winrt::Windows::Foundation::Collections::IObservableVector<SnapLayout::WindowModel> m_windows = winrt::single_threaded_observable_vector<SnapLayout::WindowModel>();
        std::unordered_map<HWND, winrt::weak_ref<SnapLayout::WindowModel>> m_windowRef; // hwnd <-> index in the IObservableVector
        HWND m_hwnd;

        void initWindows();
    };
}

namespace winrt::SnapLayout::factory_implementation
{
    struct OverviewWindow : OverviewWindowT<OverviewWindow, implementation::OverviewWindow>
    {
    };
}
