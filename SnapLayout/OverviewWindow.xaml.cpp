#include "pch.h"
#include "OverviewWindow.xaml.h"
#if __has_include("OverviewWindow.g.cpp")
#include "OverviewWindow.g.cpp"
#endif
#include "AcrylicVisualWindow.xaml.h"
#include <winrt/Microsoft.UI.Xaml.Hosting.h>
#include "WindowDragEventListener.h"
#include "WindowUtils.hpp"
#include "Interop.hpp"
#include "DirectXFactory.h"
#include "OverviewWindowFilter.h"
#include <winrt/Microsoft.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <random>
#include "WindowModel.h"
#include <winrt/Microsoft.UI.Windowing.h>

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::SnapLayout::implementation
{
	HWND OverviewWindow::Instance;
	IDCompositionDesktopDevice* OverviewWindow::dcompDevice;

	OverviewWindow::OverviewWindow()
	{
		m_hwnd = GetHwnd(*this);
		AppWindow().IsShownInSwitchers(false);
		Instance = m_hwnd;
		AddToFilter(m_hwnd);

		auto interopCompositorFactory = winrt::get_activation_factory<winrt::Windows::UI::Composition::Compositor, IInteropCompositorFactoryPartner>();
		winrt::com_ptr<IInteropCompositorPartner> interopCompositor;
		winrt::check_hresult(interopCompositorFactory->CreateInteropCompositor(
			DirectXFactory::d2dDevice.get(),
			nullptr,
			IID_PPV_ARGS(interopCompositor.put())));


		 dcompDevice = interopCompositor.as<IDCompositionDesktopDevice>().detach();
	}

	void OverviewWindow::RootGrid_Loaded(
		winrt::Windows::Foundation::IInspectable const& sender,
		winrt::Microsoft::UI::Xaml::RoutedEventArgs const&)
	{
		auto element = sender.as<winrt::Microsoft::UI::Xaml::UIElement>();
		auto visual = winrt::Microsoft::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(element);
		auto compositor = visual.Compositor();

		m_backdropLink = winrt::Microsoft::UI::Content::ContentExternalBackdropLink::Create(compositor);
		m_backdropLink.ExternalBackdropBorderMode(winrt::Microsoft::UI::Composition::CompositionBorderMode::Soft);
		m_controller = {};
		m_controller.AddSystemBackdropTarget(m_backdropLink.as<winrt::Microsoft::UI::Composition::ICompositionSupportsSystemBackdrop>());
		m_controller.SetSystemBackdropConfiguration(AcrylicVisualWindow::m_configuration);
		m_controller.Kind(winrt::Microsoft::UI::Composition::SystemBackdrops::DesktopAcrylicKind::Thin);
		m_acrylicVisual = RoundedAcrylicVisual{ m_backdropLink.PlacementVisual(), compositor, AcrylicVisualWindow::CornerRadius };
		winrt::Microsoft::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(element, m_acrylicVisual);
		
		//TODO: debug only
		//ShowAndPlaceWindowAsync({ .width = 1000, .height = 2100 });
	}

	void OverviewWindow::SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
	{
		auto control = sender.as<winrt::SnapLayout::ThumbnailVisualControl>();
		auto width = control.ActualWidth();
		control.Parent().as<winrt::Microsoft::UI::Xaml::Controls::Grid>().MaxWidth(width);
	}

	winrt::Windows::Foundation::Collections::IObservableVector<SnapLayout::WindowModel> OverviewWindow::Windows()
	{
		return m_windows;
	}

	void OverviewWindow::OnWindowCreated(HWND createdWindow)
	{
		if (IsInFilter(createdWindow))
			return;

		SnapLayout::WindowModel model{ reinterpret_cast<uint64_t>(createdWindow) };
		m_windows.Append(model);
		m_windowRef[createdWindow] = winrt::make_weak(model);
	}

	void OverviewWindow::OnWindowDestroyed(HWND destroyedWindow)
	{
		if (auto iter = m_windowRef.find(destroyedWindow); iter != m_windowRef.end())
		{
			uint32_t index{};
			m_windows.IndexOf(iter->second.get(), index);
			m_windows.RemoveAt(index);
			m_windowRef.erase(destroyedWindow);
		}
	}

	void OverviewWindow::WindowThumbnail_Click(
		winrt::Windows::Foundation::IInspectable const& sender,
		winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
	)
	{
		auto windowModel = sender.as<winrt::Microsoft::UI::Xaml::Controls::Button>()
			.DataContext()
			.as<winrt::SnapLayout::WindowModel>();
		//Do not use winrt::check_bool here because the window might be invalid
		SetWindowPos(
			winrt::get_self<WindowModel>(windowModel)->m_hwnd,
			nullptr,
			m_windowPlacement.x,
			m_windowPlacement.y,
			m_windowPlacement.width,
			m_windowPlacement.height,
			0
		);
		m_isWindowSelected = true;
		Hide();
		winrt::check_bool(SetEvent(m_windowSelectedEvent.get()));
	}

	winrt::Windows::Foundation::IAsyncOperation<bool> OverviewWindow::ShowAndPlaceWindowAsync(LayoutResult layout)
	{
		m_windowPlacement = layout;
		m_isWindowSelected = false;
		m_windowSelectedEvent.attach(CreateEvent(nullptr, false, false, nullptr));
		initWindows();
		WindowDragEventListener::SubscribeWindowEvent(this);
		Activate();
		SetWindowPos(m_hwnd, nullptr, layout.x, layout.y, layout.width, layout.height, 0);
		m_acrylicVisual.StartSizeAnimation({ 10.f, 10.f }, { layout.width, layout.height });
		co_await winrt::resume_on_signal(m_windowSelectedEvent.get());
		co_return m_isWindowSelected;
	}

	void OverviewWindow::Hide()
	{
		WindowDragEventListener::UnsubscribeWindowEvent(this);
		ShowWindow(m_hwnd, SW_HIDE);
		m_windows.Clear();
		m_windowRef.clear();
	}

	void OverviewWindow::initWindows()
	{
		struct EnumData
		{
			DWORD explorerPid{};
			OverviewWindow* self;

			EnumData(OverviewWindow* self) : self{self}
			{
				HWND hShellWnd = FindWindow(L"Shell_TrayWnd", nullptr);
				GetWindowThreadProcessId(hShellWnd, &explorerPid);
			}
		} data{ this };

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
				if (std::wstring_view{ className } == L"ApplicationFrameWindow" && pid == enumData->explorerPid)
					return true;

				enumData->self->OnWindowCreated(hwnd);
				return true;
			},
			reinterpret_cast<LPARAM>(&data)
		);
	}

	//You cannot pass in an element that uses PlacementVisual
	static winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::UI::Composition::CompositionSurfaceBrush> getBrushFromElement(
		winrt::Microsoft::UI::Xaml::UIElement const& element,
		winrt::Microsoft::UI::Composition::Compositor const& compositor)
	{
		winrt::Microsoft::UI::Xaml::Media::Imaging::RenderTargetBitmap bitmap;
		co_await bitmap.RenderAsync(element);
		auto pixelBuffer = co_await bitmap.GetPixelsAsync();		
		auto const length = pixelBuffer.Length();
		winrt::Windows::Storage::Streams::InMemoryRandomAccessStream stream;
		auto encoder = co_await winrt::Windows::Graphics::Imaging::BitmapEncoder::CreateAsync(
			winrt::Windows::Graphics::Imaging::BitmapEncoder::PngEncoderId(),
			stream
		);
		encoder.SetPixelData(
			winrt::Windows::Graphics::Imaging::BitmapPixelFormat::Bgra8,
			winrt::Windows::Graphics::Imaging::BitmapAlphaMode::Premultiplied,
			bitmap.PixelWidth(),
			bitmap.PixelHeight(),
			96,
			96,
			{ pixelBuffer.data(), pixelBuffer.data() + length }
		);

		co_await encoder.FlushAsync();
		stream.Seek(0);
		auto surface = winrt::Microsoft::UI::Xaml::Media::LoadedImageSurface::StartLoadFromStream(stream);
		co_return compositor.CreateSurfaceBrush(surface);
	}

	static winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::UI::Composition::CompositionSurfaceBrush> getBrushFromWritableBitmap(
		winrt::Microsoft::UI::Xaml::Media::Imaging::WriteableBitmap const& bitmap,
		winrt::Microsoft::UI::Composition::Compositor const& compositor
	)
	{
		auto buffer = bitmap.PixelBuffer();
	}

	static winrt::fire_and_forget playWindowClosedAnimation(winrt::Microsoft::UI::Xaml::Controls::Button const& button)
	{
		auto element = button
			.Parent()
			.as<winrt::Microsoft::UI::Xaml::Controls::Grid>()
			.Children()
			.GetAt(2)
			.as<winrt::SnapLayout::ThumbnailVisualControl>();
		//auto visual = winrt::Microsoft::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(element);
		auto visual = element.PlacementVisual();
		auto compositor = visual.Compositor();

		constexpr static auto row = 5;
		constexpr static auto column = 5;
		constexpr static std::chrono::milliseconds duration{ 1500 };

		auto brush = co_await getBrushFromElement(element, compositor);
		auto container = compositor.CreateContainerVisual();
		auto children = container.Children();

		auto size = element.ActualSize();
		auto spriteSize = size / 5.f;

		static std::mt19937 eng;
		std::uniform_real_distribution<float> upPosition{ 50.f, 100.f };
		std::uniform_real_distribution<float> downPosition{ -100.f, 100.f };
		std::uniform_real_distribution<float> rotation{ -360.f * 3, 360.f * 3 };
		brush.Scale({ 0.2f, 0.2f });
		for (auto i = 0; i < row; ++i)
		{
			for (auto j = 0; j < column; ++j)
			{
				auto sprite = compositor.CreateSpriteVisual();
				sprite.Brush(brush);
				sprite.Size(spriteSize);

				//animation
				winrt::Windows::Foundation::Numerics::float3 const originalPosition{
					i * spriteSize.x,
					j * spriteSize.y,
					0.f
				};
				auto offsetAnimation = compositor.CreateVector3KeyFrameAnimation();
				offsetAnimation.InsertKeyFrame(0.f, originalPosition);
				offsetAnimation.InsertKeyFrame(0.2f, originalPosition + winrt::Windows::Foundation::Numerics::float3{ upPosition(eng), upPosition(eng), 0.f });
				offsetAnimation.InsertKeyFrame(1.f, originalPosition + winrt::Windows::Foundation::Numerics::float3{ downPosition(eng), size.y * 2, 0.f });
				offsetAnimation.Duration(duration);

				auto rotationAnimation = compositor.CreateScalarKeyFrameAnimation();
				rotationAnimation.InsertKeyFrame(1.f, rotation(eng));
				rotationAnimation.Duration(duration);

				sprite.StartAnimation(L"Offset", offsetAnimation);
				sprite.StartAnimation(L"RotationAngleInDegrees", rotationAnimation);

				//add to container
				children.InsertAtTop(sprite);
			}
		}
		winrt::Microsoft::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(element, container);
	}

	void OverviewWindow::CloseWindowButton_Click(
		winrt::Windows::Foundation::IInspectable const& sender, 
		winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
	{
		auto button = sender.as<winrt::Microsoft::UI::Xaml::Controls::Button>();
		auto model = button.DataContext().as<winrt::SnapLayout::WindowModel>();
		SendMessage(winrt::get_self<WindowModel>(model)->m_hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
		//playWindowClosedAnimation(button);
	}

	void OverviewWindow::Window_Activated(
		winrt::Windows::Foundation::IInspectable const& sender, 
		winrt::Microsoft::UI::Xaml::WindowActivatedEventArgs const& args)
	{
		if (args.WindowActivationState() != winrt::Microsoft::UI::Xaml::WindowActivationState::Deactivated)
			return;
		
		Hide();
		winrt::check_bool(SetEvent(m_windowSelectedEvent.get()));
		args.Handled(true);
	}
}
