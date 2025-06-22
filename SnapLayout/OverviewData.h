#pragma once
#include "INotifyWindowEvent.hpp"
#include <winrt/SnapLayout.h>

class OverviewData : public INotifyWindowEvent
{
	winrt::Windows::Foundation::Collections::IObservableVector<winrt::SnapLayout::WindowModel> m_windows = winrt::single_threaded_observable_vector<winrt::SnapLayout::WindowModel>();
	std::unordered_map<HWND, winrt::weak_ref<winrt::SnapLayout::WindowModel>> m_windowRef; // hwnd <-> index in the IObservableVector
public:
	void Clear();
	void Refresh(HWND excludeWindow);

	void OnWindowCreated(HWND createdWindow) override;
	void OnWindowDestroyed(HWND destroyedWindow) override;

	winrt::Windows::Foundation::Collections::IObservableVector<winrt::SnapLayout::WindowModel> Get() const;
};
