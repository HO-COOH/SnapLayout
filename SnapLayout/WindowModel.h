#pragma once

#include "WindowModel.g.h"

namespace winrt::SnapLayout::implementation
{
    struct WindowModel : WindowModelT<WindowModel>
    {
        WindowModel(uint64_t handle);

        uint64_t Handle();

        winrt::hstring Title();

    private:
        HWND m_hwnd;
    };
}

namespace winrt::SnapLayout::factory_implementation
{
    struct WindowModel : WindowModelT<WindowModel, implementation::WindowModel>
    {
    };
}
