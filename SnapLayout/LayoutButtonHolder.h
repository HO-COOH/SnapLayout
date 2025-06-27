#pragma once
#include <winrt/Microsoft.UI.Xaml.Controls.h>

/**
 * @brief This class automatically manages the VisualState of a LayoutButton
 */
class LayoutButtonHolder
{
	winrt::Microsoft::UI::Xaml::Controls::Button m_button{ nullptr };
public:
	bool operator!=(winrt::Microsoft::UI::Xaml::Controls::Button const& other) const noexcept;
	operator bool() const noexcept;

	LayoutButtonHolder& operator=(winrt::Microsoft::UI::Xaml::Controls::Button const& button) noexcept;
	LayoutButtonHolder& operator=(std::nullptr_t) noexcept;
	[[nodiscard]] winrt::Microsoft::UI::Xaml::Controls::Button const& Get() const noexcept;
};
