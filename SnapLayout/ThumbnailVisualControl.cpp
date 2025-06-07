#include "pch.h"
#include "ThumbnailVisualControl.h"
#if __has_include("ThumbnailVisualControl.g.cpp")
#include "ThumbnailVisualControl.g.cpp"
#endif
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Composition.h>
#include "OverviewWindow.xaml.h"
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include "DebugHelper.hpp"

namespace winrt::SnapLayout::implementation
{
	ThumbnailVisualControl::ThumbnailVisualControl()
	{
		Loaded([this](auto&&...) {
			updateVisual();
		});
	}

	uint64_t ThumbnailVisualControl::Handle()
	{
		return reinterpret_cast<uint64_t>(m_handle);
	}

	void ThumbnailVisualControl::Handle(uint64_t value)
	{
		m_handle = reinterpret_cast<HWND>(value);

		//TODO: remove on release
		wchar_t title[MAX_PATH]{};
		GetWindowText(m_handle, title, std::size(title));

		RECT originalWindowRect;
		winrt::check_bool(GetWindowRect(m_handle, &originalWindowRect));

		Width(ShrinkPercentage * (originalWindowRect.right - originalWindowRect.left));
		Height(ShrinkPercentage * (originalWindowRect.bottom - originalWindowRect.top));

		DebugLog(L"{:x} {} : W: {}, H: {}\n", value, title, originalWindowRect.right - originalWindowRect.left, originalWindowRect.bottom - originalWindowRect.top);
	}

	void ThumbnailVisualControl::updateVisual()
	{
		if (!m_handle)
			return;

		if (!m_outputLink)
		{
			auto compositor = winrt::Microsoft::UI::Xaml::Media::CompositionTarget::GetCompositorForCurrentThread();
			m_outputLink = winrt::Microsoft::UI::Content::ContentExternalOutputLink::Create(compositor);
			m_thumbnail = ThumbnailVisual{
				m_handle,
				OverviewWindow::Instance,
				OverviewWindow::dcompDevice,
			};
			m_outputLink.as<winrt::Windows::UI::Composition::CompositionTarget>().Root(m_thumbnail);
			auto placementVisual = m_outputLink.PlacementVisual();
			m_thumbnail.Scale({ ShrinkPercentage, ShrinkPercentage, 1.f });
			auto size = ActualSize();
			placementVisual.Size(size);
			winrt::Microsoft::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(*this, placementVisual);
		}
	}

	//winrt::Windows::Foundation::Size ThumbnailVisualControl::MeasureOverride(winrt::Windows::Foundation::Size finalSize)
	//{
	//	RECT originalWindowRect;
	//	winrt::check_bool(GetWindowRect(m_handle, &originalWindowRect));
	//	return winrt::Windows::Foundation::Size{
	//		ShrinkPercentage* (originalWindowRect.right - originalWindowRect.left),
	//		ShrinkPercentage* (originalWindowRect.bottom - originalWindowRect.top)
	//	};
	//}
}
