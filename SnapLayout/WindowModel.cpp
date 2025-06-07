#include "pch.h"
#include "WindowModel.h"
#if __has_include("WindowModel.g.cpp")
#include "WindowModel.g.cpp"
#endif


namespace winrt::SnapLayout::implementation
{
	WindowModel::WindowModel(uint64_t handle) : m_hwnd{ reinterpret_cast<HWND>(handle) }
	{
	}

	uint64_t WindowModel::Handle()
	{
		return reinterpret_cast<uint64_t>(m_hwnd);
	}

	winrt::hstring WindowModel::Title()
	{
		wchar_t title[MAX_PATH]{};
		GetWindowText(m_hwnd, title, MAX_PATH);
		return winrt::hstring{ title };
	}
}
