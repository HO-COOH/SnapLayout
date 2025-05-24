#pragma once
#include <winrt/Microsoft.UI.Composition.h>

class RoundedAcrylicVisual
{
	winrt::Microsoft::UI::Composition::Visual m_placementVisual{ nullptr };
	winrt::Microsoft::UI::Composition::RectangleClip m_clip{ nullptr };
	winrt::Microsoft::UI::Composition::Compositor m_compositor{ nullptr };
public:
	RoundedAcrylicVisual(std::nullptr_t) {}

	RoundedAcrylicVisual(
		winrt::Microsoft::UI::Composition::Visual const& placementVisual,
		winrt::Microsoft::UI::Composition::Compositor const& compositor,
		float cornerRadius);

	void StartSizeAnimation(
		winrt::Windows::Foundation::Numerics::float2 from,
		winrt::Windows::Foundation::Numerics::float2 to
	);

	void StartOffsetAnimation(
		winrt::Windows::Foundation::Numerics::float2 from,
		winrt::Windows::Foundation::Numerics::float2 to
	);

	operator winrt::Microsoft::UI::Composition::Visual() const
	{
		return m_placementVisual;
	}

	void Hide();
};

