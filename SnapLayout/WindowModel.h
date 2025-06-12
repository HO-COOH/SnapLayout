#pragma once

#include "WindowModel.g.h"
#include <include/PropertyChangeHelper.hpp>
#include "LayoutResult.h"

namespace winrt::SnapLayout::implementation
{
    struct WindowModel : WindowModelT<WindowModel>, MvvmHelper::PropertyChangeHelper<WindowModel>
    {
        WindowModel(uint64_t handle);

        uint64_t Handle();

        winrt::hstring Title();

        winrt::Microsoft::UI::Xaml::Media::ImageSource Icon();

        void Close();

        void SetWindowPos(LayoutResult layout);
    private:
        HWND m_hwnd;
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
