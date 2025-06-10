#include "pch.h"
#include "App.xaml.h"
#include "MainWindow.xaml.h"
#include <winrt/Windows.UI.Core.h>
#include <inspectable.h>
#include "WindowDragEventListener.h"
#include "ThumbnailVisualContainerWindow.h"
#include "MouseHookDll.h"
#include "Interop.hpp"


// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.
 

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
            MouseHookDll::Unset();
            WindowDragEventListener::Unset();
            MessageBox(NULL, e.Message().data(), L"An internal error happened and SnapLayout need to exit.", NULL);
            if (IsDebuggerPresent())
            {
                auto errorMessage = e.Message();
                __debugbreak();
            }
            ExitProcess(1);
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
        winrt::SnapLayout::AcrylicVisualWindow{};
        MouseHookDll::Set(MainWindow::g_instance);
        WindowDragEventListener::Set();

        //TODO: Debug only
        //ThumbnailVisualContainerWindow::Instance().SetVisual(
        //    FindWindowW(L"Shell_TrayWnd", nullptr),
        //    { 0, 0, 300, 300 }
        //);

        ThumbnailVisualContainerWindow::Instance().Hide();
    }
}
