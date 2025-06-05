#include "pch.h"
#include "LayoutResult.h"
#include "DpiUtils.hpp"

void LayoutResult::UnscaleForDpi(UINT dpi)
{
	x = ::UnscaleForDpi(x, dpi);
	width = ::UnscaleForDpi(width, dpi);
	y = ::UnscaleForDpi(y, dpi);
	height = ::UnscaleForDpi(height, dpi);
}
