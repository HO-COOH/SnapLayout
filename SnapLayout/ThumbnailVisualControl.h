#pragma once

#include "ThumbnailVisualControl.g.h"
#include <winrt/Microsoft.UI.Content.h>
#include "ThumbnailVisual.h" 

namespace winrt::SnapLayout::implementation
{
    struct ThumbnailVisualControl : ThumbnailVisualControlT<ThumbnailVisualControl>
    {
        ThumbnailVisualControl();

        uint64_t Handle();
		void Handle(uint64_t value);

        //winrt::Windows::Foundation::Size ArrangeOverride(winrt::Windows::Foundation::Size finalSize);
        //winrt::Windows::Foundation::Size MeasureOverride(winrt::Windows::Foundation::Size availableSize);
        winrt::Microsoft::UI::Composition::Visual PlacementVisual();
    private:
        HWND m_handle{};
        void updateVisual();
        winrt::Microsoft::UI::Content::ContentExternalOutputLink m_outputLink{ nullptr };
        winrt::Microsoft::UI::Composition::Visual m_placementVisual{ nullptr };
        ThumbnailVisual m_thumbnail{ nullptr };
        constexpr static auto ShrinkPercentage = 0.2f;
    };
}

namespace winrt::SnapLayout::factory_implementation
{
    struct ThumbnailVisualControl : ThumbnailVisualControlT<ThumbnailVisualControl, implementation::ThumbnailVisualControl>
    {
    };
}
