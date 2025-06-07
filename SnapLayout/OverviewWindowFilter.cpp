#include "pch.h"
#include "OverviewWindowFilter.h"
#include <unordered_set>

static std::unordered_set<HWND> filterWindows;

void AddToFilter(HWND hwnd)
{
	filterWindows.insert(hwnd);
}

void RemoveFromFilter(HWND hwnd)
{
	filterWindows.erase(hwnd);
}

bool IsInFilter(HWND hwnd)
{
	return filterWindows.find(hwnd) != filterWindows.end();
}
