#pragma once
#include <Windows.h>

template<typename T = int>
auto ScaleForDpi(auto value, UINT dpi)
{
	return static_cast<T>(static_cast<double>(value) * dpi / 96.0);
}

template<typename T = float>
T UnscaleForDpi(auto value, UINT dpi)
{
	return value * 96.0 / dpi;
}


template<typename T = winrt::Windows::Foundation::Point>
T UnscalePointForDpi(POINT p, UINT dpi)
{
	return T{
		UnscaleForDpi(p.x, dpi),
		UnscaleForDpi(p.y, dpi)
	};
}
