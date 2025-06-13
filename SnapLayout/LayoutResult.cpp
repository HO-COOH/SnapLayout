#include "pch.h"
#include "LayoutResult.h"
#include "DpiUtils.hpp"
#include <winrt/Microsoft.UI.Xaml.h>

void LayoutResult::UnscaleForDpi(UINT dpi)
{
	x = ::UnscaleForDpi(x, dpi);
	width = ::UnscaleForDpi(width, dpi);
	y = ::UnscaleForDpi(y, dpi);
	height = ::UnscaleForDpi(height, dpi);
}

void LayoutResult::AddPadding(float uniformPadding)
{
	x += uniformPadding;
	y += uniformPadding;
	width -= 2 * uniformPadding;
	height -= 2 * uniformPadding;
}

void LayoutResult::AddPadding(winrt::Microsoft::UI::Xaml::Thickness padding)
{
	x += padding.Left;
	y += padding.Top;
	width -= (padding.Left + padding.Right);
	height -= (padding.Top + padding.Bottom);
}
