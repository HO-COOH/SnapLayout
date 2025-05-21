#pragma once
#include <Windows.h>

template<typename T = int>
auto ScaleForDpi(auto value, UINT dpi)
{
	return static_cast<T>(static_cast<double>(value) * dpi / 96.0);
}

auto UnscaleForDpi(auto value, UINT dpi)
{
	return value * 96.0 / dpi;
}
