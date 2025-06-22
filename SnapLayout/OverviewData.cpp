#include "pch.h"
#include "WindowUtils.hpp"
#include "OverviewData.h"
#include "OverviewWindowFilter.h"


void OverviewData::Refresh(HWND excludeWindow)
{
	struct EnumData
	{
		DWORD explorerPid{};
		OverviewData* self;
		HWND excludeWindow;

		EnumData(OverviewData* self, HWND excludeWindow) : self{ self }, excludeWindow{ excludeWindow }
		{
			HWND hShellWnd = FindWindow(L"Shell_TrayWnd", nullptr);
			GetWindowThreadProcessId(hShellWnd, &explorerPid);
		}
	} data{ this, excludeWindow };

	EnumWindows(
		+[](HWND hwnd, LPARAM lparam) -> BOOL
		{
			if (!IsWindowResizable(hwnd, true) || IsInFilter(hwnd))
				return true;

			wchar_t className[MAX_PATH]{};
			GetClassName(hwnd, className, std::size(className));
			DWORD pid;
			GetWindowThreadProcessId(hwnd, &pid);

			auto enumData = reinterpret_cast<EnumData*>(lparam);
			if ((std::wstring_view{ className } == L"ApplicationFrameWindow" && pid == enumData->explorerPid) || hwnd == enumData->excludeWindow)
				return true;

			enumData->self->OnWindowCreated(hwnd);
			return true;
		},
		reinterpret_cast<LPARAM>(&data)
	);
}

void OverviewData::Clear()
{
	m_windows.Clear();
	m_windowRef.clear();
}

void OverviewData::OnWindowCreated(HWND createdWindow)
{
	if (IsInFilter(createdWindow))
		return;

	winrt::SnapLayout::WindowModel model{ reinterpret_cast<uint64_t>(createdWindow) };
	m_windows.Append(model);
	m_windowRef[createdWindow] = winrt::make_weak(model);
}

void OverviewData::OnWindowDestroyed(HWND destroyedWindow)
{
	if (auto iter = m_windowRef.find(destroyedWindow); iter != m_windowRef.end())
	{
		uint32_t index{};
		m_windows.IndexOf(iter->second.get(), index);
		m_windows.RemoveAt(index);
		m_windowRef.erase(destroyedWindow);
	}
}

winrt::Windows::Foundation::Collections::IObservableVector<winrt::SnapLayout::WindowModel> OverviewData::Get() const
{
	return m_windows;
}