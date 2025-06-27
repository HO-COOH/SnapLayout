#include "pch.h"
#include "LayoutButtonHolder.h"

bool LayoutButtonHolder::operator!=(winrt::Microsoft::UI::Xaml::Controls::Button const& other) const noexcept
{
	return m_button != other;
}

LayoutButtonHolder& LayoutButtonHolder::operator=(winrt::Microsoft::UI::Xaml::Controls::Button const& button) noexcept
{
	if(m_button)
		winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(m_button, L"Normal", true);
	winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(button, L"PointerOver", true);
	m_button = button;
	return *this;
}

LayoutButtonHolder& LayoutButtonHolder::operator=(std::nullptr_t) noexcept
{
	if (m_button)
		winrt::Microsoft::UI::Xaml::VisualStateManager::GoToState(m_button, L"Normal", true);
	m_button = nullptr;
	return *this;
}

[[nodiscard]] winrt::Microsoft::UI::Xaml::Controls::Button const& LayoutButtonHolder::Get() const noexcept
{
	return m_button;
}

LayoutButtonHolder::operator bool() const noexcept
{
	return m_button != nullptr;
}