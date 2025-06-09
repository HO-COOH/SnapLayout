#pragma once

#include "WindowModel.g.h"
#include <include/PropertyChangeHelper.hpp>

namespace winrt::SnapLayout::implementation
{
    struct WindowModel : WindowModelT<WindowModel>, MvvmHelper::PropertyChangeHelper<WindowModel>
    {
        WindowModel(uint64_t handle);

        uint64_t Handle();

        winrt::hstring Title();

        winrt::Microsoft::UI::Xaml::Media::ImageSource Icon();

        HWND m_hwnd;
    private:
        void loadIcon();
        winrt::Microsoft::UI::Xaml::Media::ImageSource m_icon{ nullptr };
    };
}

namespace winrt::SnapLayout::factory_implementation
{
    struct WindowModel : WindowModelT<WindowModel, implementation::WindowModel>
    {
    };
}
