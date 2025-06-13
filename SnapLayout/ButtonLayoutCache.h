#pragma once
#include <unordered_map>
#include "LayoutResult.h"
#include <winrt/Microsoft.UI.Xaml.Controls.h>

/**
 *  Using a cache to store the layout result of buttons will save us from ~20 microseconds to <1 microseconds on my PC
 *  You can comment out the ScopeTimer in `GetLayout()` to see the difference
 */
class ButtonLayoutCache
{
	std::unordered_map<winrt::Microsoft::UI::Xaml::Controls::Button, LayoutResult> cache;

	static [[nodiscard]] LayoutResult calculateLayout(winrt::Microsoft::UI::Xaml::Controls::Button const& button);
public:
	[[nodiscard]] LayoutResult GetLayout(winrt::Microsoft::UI::Xaml::Controls::Button const& button);
};
