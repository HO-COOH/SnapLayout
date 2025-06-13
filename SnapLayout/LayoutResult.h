#pragma once

struct LayoutResult
{
	float x;
	float y;
	float width;
	float height;

	void UnscaleForDpi(UINT dpi);
	
	void AddPadding(float uniformPadding);
	void AddPadding(winrt::Microsoft::UI::Xaml::Thickness padding);
};
