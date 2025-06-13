#include "pch.h"
#include "ButtonLayoutCache.h"

LayoutResult ButtonLayoutCache::calculateLayout(winrt::Microsoft::UI::Xaml::Controls::Button const& button)
{
	auto parentGrid = button.Parent().as<winrt::Microsoft::UI::Xaml::Controls::Grid>();
	/*Instead of calculating rowTotal, buttonRow in two pass, we can hand write one for loop, better performance*/

	auto const buttonRow = winrt::Microsoft::UI::Xaml::Controls::Grid::GetRow(button);
	auto const buttonRowSpan = winrt::Microsoft::UI::Xaml::Controls::Grid::GetRowSpan(button);

	float rowBeforeButton{}, rowTotal{}, buttonRowTotal{};
	for (int i = 0; auto row : parentGrid.RowDefinitions())
	{
		float const current = row.Height().Value;
		rowTotal += current;
		if (i < buttonRow)
			rowBeforeButton += current;
		else if (i >= buttonRow && i < buttonRow + buttonRowSpan)
			buttonRowTotal += current;
		++i;
	}
	if (rowTotal == 0)
	{
		//means we only have 1 row, and the button is occupying the whole row
		rowTotal = 1;
		buttonRowTotal = 1;
	}

	auto const buttonCol = winrt::Microsoft::UI::Xaml::Controls::Grid::GetColumn(button);
	auto const buttonColSpan = winrt::Microsoft::UI::Xaml::Controls::Grid::GetColumnSpan(button);
	float colBeforeButton{}, colTotal{}, buttonColTotal{};
	for (int i = 0; auto col : parentGrid.ColumnDefinitions())
	{
		float const current = col.Width().Value;
		colTotal += current;
		if (i < buttonCol)
			colBeforeButton += current;
		else if (i >= buttonCol && i < buttonCol + buttonColSpan)
			buttonColTotal += current;
		++i;
	}

	if (colTotal == 0)
	{
		colTotal = 1;
		buttonColTotal = 1;
	}

	return LayoutResult{
		.x = (colBeforeButton / colTotal),
		.y = (rowBeforeButton / rowTotal),
		.width = (buttonColTotal / colTotal),
		.height = (buttonRowTotal / rowTotal)
	};
}

LayoutResult ButtonLayoutCache::GetLayout(winrt::Microsoft::UI::Xaml::Controls::Button const& button)
{
	class ScopedTimer
	{
		std::chrono::steady_clock::time_point m_startTime = std::chrono::steady_clock::now();
	public:
		~ScopedTimer()
		{
			auto const endTime = std::chrono::steady_clock::now();
			OutputDebugStringA(std::format("Took {} ms\n", std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_startTime).count()).data());
		}
	};
	//ScopedTimer t;

	if (auto cached = cache.find(button); cached != cache.end())
		return cached->second;


	auto const result = calculateLayout(button);
	cache[button] = result;
	return result;
}
