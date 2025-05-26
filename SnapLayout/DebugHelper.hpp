#pragma once

#if (defined _DEBUG) || (defined DEBUG)
#include <format>
#include <debugapi.h>
#endif

template <typename... Args>
void DebugLog(std::wformat_string<Args...> fmt, Args&&... args)
{
#if (defined _DEBUG) || (defined DEBUG)
	if constexpr (sizeof...(args) == 0)
	{
		OutputDebugStringW(fmt.get().data());
	}
	else
	{
		OutputDebugStringW(std::format(fmt, std::forward<Args>(args)...).data());
	}
#endif
}

template <typename... Args>
void DebugLog(std::format_string<Args...> fmt, Args&&... args)
{
#if (defined _DEBUG) || (defined DEBUG)
	if constexpr (sizeof...(args) == 0)
	{
		OutputDebugStringA(fmt.get().data());
	}
	else
	{
		OutputDebugStringA(std::format(fmt, std::forward<Args>(args)...).data());
	}
#endif
}
