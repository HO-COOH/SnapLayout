#pragma once

#if (defined _DEBUG) || (defined DEBUG)
#include <format>
#include <debugapi.h>
#endif

template <typename... Args>
void DebugLog([[maybe_unused]] std::wformat_string<Args...> fmt, [[maybe_unused]] Args&&... args)
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

inline void DebugLog(const char* msg)
{
#if (defined _DEBUG) || (defined DEBUG)
	OutputDebugStringA(msg);
#endif
}

inline void DebugLog(const wchar_t* msg)
{
#if (defined _DEBUG) || (defined DEBUG)
	OutputDebugStringW(msg);
#endif
}

template <typename... Args>
void DebugLog([[maybe_unused]] std::format_string<Args...> fmt, [[maybe_unused]] Args&&... args)
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
