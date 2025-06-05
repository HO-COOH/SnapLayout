#pragma once
#include <windows.h>
#include "LayoutResult.h"

//Return a layout for window placement
[[nodiscard]] LayoutResult LayoutImpl(LayoutResult layoutResult, HMONITOR monitor, HWND draggedWindow);

void ConvertLayoutToMonitorWindowPlacement(LayoutResult& layoutResult, RECT rcWork);
void ConvertLayoutToMonitorWindowPlacement(LayoutResult& layoutResult, HMONITOR monitor);
