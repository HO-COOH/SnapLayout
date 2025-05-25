#pragma once
#include <combaseapi.h>
#include <Unknwn.h>
#include <inspectable.h>
#include <winrt/Windows.UI.Core.h>
#include <windows.ui.composition.h>
#include <dcomp.h>

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

//-------- Interop Composition interfaces

//Windows::UI::Composition::HwndTarget (14393-15063)
DECLARE_INTERFACE_IID_(HwndTarget, IUnknown, "6677DA68-C80C-407A-A4D2-3AA118AD7C46")
{
    STDMETHOD(GetRoot)(THIS_
        OUT ABI::Windows::UI::Composition::IVisual * *value) PURE;
    STDMETHOD(SetRoot)(THIS_
        IN ABI::Windows::UI::Composition::IVisual * value) PURE;
};

//Windows::UI::Composition::InteropCompositorTarget
DECLARE_INTERFACE_IID_(InteropCompositionTarget, IUnknown, "EACDD04C-117E-4E17-88F4-D1B12B0E3D89")
{
    STDMETHOD(SetRoot)(THIS_
        IN IDCompositionVisual * visual) PURE;
};

//Windows::UI::Composition::IInteropCompositorPartner
DECLARE_INTERFACE_IID_(IInteropCompositorPartner, IUnknown, "e7894c70-af56-4f52-b382-4b3cd263dc6f")
{
    STDMETHOD(MarkDirty)(THIS_) PURE;

    STDMETHOD(ClearCallback)(THIS_) PURE;

    STDMETHOD(CreateManipulationTransform)(THIS_
        IN IDCompositionTransform * transform,
        IN REFIID iid,
        OUT VOID * *result) PURE;

    STDMETHOD(RealClose)(THIS_) PURE;
};

//Windows.UI.Composition.IInteropCompositorPartnerCallback
DECLARE_INTERFACE_IID_(IInteropCompositorPartnerCallback, IUnknown, "9bb59fc9-3326-4c32-bf06-d6b415ac2bc5")
{
    STDMETHOD(NotifyDirty)(THIS_) PURE;

    STDMETHOD(NotifyDeferralState)(THIS_
        bool deferRequested) PURE;
};

//Windows::UI::Composition::IInteropCompositorFactoryPartner
DECLARE_INTERFACE_IID_(IInteropCompositorFactoryPartner, IInspectable, "22118adf-23f1-4801-bcfa-66cbf48cc51b")
{
    STDMETHOD(CreateInteropCompositor)(THIS_
        IN IUnknown * renderingDevice,
        IN IInteropCompositorPartnerCallback * callback,
        IN REFIID iid,
        OUT VOID * *instance
        ) PURE;

    STDMETHOD(CheckEnabled)(THIS_
        OUT bool* enableInteropCompositor,
        OUT bool* enableExposeVisual
        ) PURE;
};

