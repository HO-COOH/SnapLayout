#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include <winrt/Windows.UI.Core.h>
#include <inspectable.h>
#include "WindowDragEventListener.h"
#include "ThumbnailVisualContainerWindow.h"

bool HasLButtonDown = false;
// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.
 

//Windows::UI::Core::IInternalCoreDispatcherStatic (14393+)
DECLARE_INTERFACE_IID_(IInternalCoreDispatcherStatic, IInspectable, "4B4D0861-D718-4F7C-BEC7-735C065F7C73")
{
    STDMETHOD(GetForCurrentThread)(
        winrt::Windows::UI::Core::CoreDispatcher * ppDispatcher
        ) PURE;
    STDMETHOD(GetOrCreateForCurrentThread)(
        winrt::Windows::UI::Core::CoreDispatcher * ppDispatcher
        ) PURE;
};

namespace winrt::SnapLayout::implementation
{
    /// <summary>
    /// Initializes the singleton application object.  This is the first line of authored code
    /// executed, and as such is the logical equivalent of main() or WinMain().
    /// </summary>
    App::App()
    {
        // Xaml objects should not call InitializeComponent during construction.
        // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
        UnhandledException([](winrt::Windows::Foundation::IInspectable const&, winrt::Microsoft::UI::Xaml::UnhandledExceptionEventArgs const& e)
        {
            if (IsDebuggerPresent())
            {
                auto errorMessage = e.Message();
                __debugbreak();
            }
        });
#endif
    }

    /// <summary>
    /// Invoked when the application is launched.
    /// </summary>
    /// <param name="e">Details about the launch request and process.</param>
    void App::OnLaunched([[maybe_unused]] winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const& e)
    {
        window = make<MainWindow>();

        using InstallHookFunc = void(*)(HWND, bool*);
        using UnsetHookFunc = void(*)();

        auto lib = LoadLibrary(L"../../MouseHook.dll");
        reinterpret_cast<InstallHookFunc>(GetProcAddress(lib, "InstallHook"))(MainWindow::g_instance, &HasLButtonDown);

        winrt::SnapLayout::AcrylicVisualWindow{};

        //CoreDispatcher internally creates a DispatcherQueue if it doesn't exist (and if you are on 15063 or newer)
        auto dispatcher = winrt::try_get_activation_factory<winrt::Windows::UI::Core::CoreDispatcher, IInternalCoreDispatcherStatic>();

        //On 10586 there isn't a way to create a CoreDispatcher (unless you create a CoreWindow, lol.)

        winrt::Windows::UI::Core::CoreDispatcher coreDispatcher{ nullptr };
        dispatcher->GetOrCreateForCurrentThread(&coreDispatcher);

        //window->SetVisual(FindWindow(L"Shell_TrayWnd", NULL));
        WindowDragEventListener::Set();

        //TODO: Debug only
        //ThumbnailVisualContainerWindow::Instance().SetVisual(
        //    FindWindowW(L"Shell_TrayWnd", nullptr),
        //    { 0, 0, 300, 300 }
        //);

        ThumbnailVisualContainerWindow::Instance();
    }
}
