#include "pch.h"
#include "WindowModel.h"
#if __has_include("WindowModel.g.cpp")
#include "WindowModel.g.cpp"
#endif
#include <wil/resource.h>
#include <shellapi.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include "MainWindow.xaml.h"
#include "DebugHelper.hpp"
#include <future>
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")

static winrt::Microsoft::UI::Xaml::Media::Imaging::WriteableBitmap HBitmapToWriteableBitmap(HBITMAP hBitmap, HBITMAP hMask)
{
	// Get the screen DC
	HDC dc = GetDC(NULL);

	// Get icon size info
	BITMAP bm = { 0 };
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	if (bm.bmWidth == 0 || bm.bmHeight == 0)
		return nullptr;

	// Set up BITMAPINFO
	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = bm.bmWidth;
	bmi.bmiHeader.biHeight = -bm.bmHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	// Extract the color bitmap
	int nBits = bm.bmWidth * bm.bmHeight;
	auto colorBits = std::make_unique<int32_t[]>(nBits);
	GetDIBits(dc, hBitmap, 0, bm.bmHeight, colorBits.get(), &bmi, DIB_RGB_COLORS);

	// Check whether the color bitmap has an alpha channel.
	BOOL hasAlpha = FALSE;
	for (int i = 0; i < nBits; i++) {
		if ((colorBits[i] & 0xff000000) != 0) {
			hasAlpha = TRUE;
			break;
		}
	}

	// If no alpha values available, apply the mask bitmap
	if (!hasAlpha) {
		// Extract the mask bitmap
		int32_t* maskBits = new int32_t[nBits];
		GetDIBits(dc, hMask, 0, bm.bmHeight, maskBits, &bmi, DIB_RGB_COLORS);
		// Copy the mask alphas into the color bits
		for (int i = 0; i < nBits; i++) {
			if (maskBits[i] == 0) {
				colorBits[i] |= 0xff000000;
			}
		}
		delete[] maskBits;
	}

	// Release DC and GDI bitmaps
	ReleaseDC(NULL, dc);
	DeleteObject(hBitmap);
	if (hMask != NULL)
		DeleteObject(hMask);


	{
		winrt::Microsoft::UI::Xaml::Media::Imaging::WriteableBitmap bitmap{ bm.bmWidth, bm.bmHeight };
		auto buffer = bitmap.PixelBuffer();
		auto pBuffer = buffer.data();
		for (int i = 0; i < nBits; i++)
		{
			*pBuffer = colorBits[i] & 0xff; ++pBuffer;
			*pBuffer = (colorBits[i] >> 8) & 0xff; ++pBuffer;
			*pBuffer = (colorBits[i] >> 16) & 0xff; ++pBuffer;
			*pBuffer = (colorBits[i] >> 24) & 0xff;

			if (i != nBits - 1) ++pBuffer;
		}
		return bitmap;
	}
}

namespace winrt::SnapLayout::implementation
{
	WindowModel::WindowModel(uint64_t handle) : m_hwnd{ reinterpret_cast<HWND>(handle) }
	{
		loadIcon();
	}

	void WindowModel::loadIcon()
	{
		try
		{
			DWORD pid;
			GetWindowThreadProcessId(m_hwnd, &pid);

			wil::unique_handle hProcess{ OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid) };
			if (!hProcess)
				return;

			wchar_t path[MAX_PATH];
			DWORD pathSize = MAX_PATH;
			winrt::check_bool(QueryFullProcessImageNameW(hProcess.get(), 0, path, &pathSize));

			//Get icon from exe path
			SHFILEINFO info{};
			winrt::check_bool(SHGetFileInfoW(path, 0, &info, sizeof(info), SHGFI_ICON | SHGFI_LARGEICON));

			ICONINFO iconInfo;
			winrt::check_bool(GetIconInfo(info.hIcon, &iconInfo));

			m_icon = HBitmapToWriteableBitmap(iconInfo.hbmColor, iconInfo.hbmMask);
		}
		catch (winrt::hresult_error const& e)
		{
			DebugLog(L"{}\n", e.message());
		}
	}

	winrt::Microsoft::UI::Xaml::Media::ImageSource WindowModel::Icon()
	{
		return m_icon;
	}

	uint64_t WindowModel::Handle()
	{
		return reinterpret_cast<uint64_t>(m_hwnd);
	}

	void WindowModel::SetWindowPos(LayoutResult layout)
	{
		//Do not use winrt::check_bool here because the window might be invalid
		::SetWindowPos(
			m_hwnd,
			nullptr,
			static_cast<int>(layout.x),
			static_cast<int>(layout.y),
			static_cast<int>(layout.width),
			static_cast<int>(layout.height),
			SWP_NOACTIVATE
		);
	}

	winrt::hstring WindowModel::Title()
	{
		wchar_t title[MAX_PATH]{};
		GetWindowText(m_hwnd, title, MAX_PATH);
		return winrt::hstring{ title };
	}

	void WindowModel::Close()
	{
		SendMessage(m_hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
	}
}
