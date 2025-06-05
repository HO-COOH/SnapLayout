#include "pch.h"
#include "RoundedAcrylicVisual.h"

RoundedAcrylicVisual::RoundedAcrylicVisual(winrt::Microsoft::UI::Composition::Visual const& placementVisual, winrt::Microsoft::UI::Composition::Compositor const& compositor, float CornerRadius) :
	m_placementVisual{ placementVisual },
	m_compositor{ compositor },
	m_clip{ compositor.CreateRectangleClip(
		0,
		0,
		10.f,
		10.f,
		{ CornerRadius, CornerRadius },
		{ CornerRadius, CornerRadius },
		{ CornerRadius, CornerRadius },
		{ CornerRadius, CornerRadius }
	) }
{
	m_placementVisual.Clip(m_clip);
}

void RoundedAcrylicVisual::StartSizeAnimation(winrt::Windows::Foundation::Numerics::float2 from, winrt::Windows::Foundation::Numerics::float2 to)
{
	auto placementVisualSizeAnimation = m_compositor.CreateVector2KeyFrameAnimation();
	placementVisualSizeAnimation.InsertKeyFrame(0.f, from);
	placementVisualSizeAnimation.InsertKeyFrame(1.f, to);
	
	auto clipRightAnimation = m_compositor.CreateScalarKeyFrameAnimation();
	clipRightAnimation.InsertKeyFrame(0.f, from.x);
	clipRightAnimation.InsertKeyFrame(1.f, to.x);

	auto clipBottomAnimation = m_compositor.CreateScalarKeyFrameAnimation();
	clipBottomAnimation.InsertKeyFrame(0.f, from.y);
	clipBottomAnimation.InsertKeyFrame(1.f, to.y);

	m_placementVisual.StartAnimation(L"Size", placementVisualSizeAnimation);
	m_clip.StartAnimation(L"Right", clipRightAnimation);
	m_clip.StartAnimation(L"Bottom", clipBottomAnimation);
}

void RoundedAcrylicVisual::StartOffsetAnimation(winrt::Windows::Foundation::Numerics::float2 from, winrt::Windows::Foundation::Numerics::float2 to)
{
	auto placementVisualOffsetAnimation = m_compositor.CreateVector2KeyFrameAnimation();
	placementVisualOffsetAnimation.InsertKeyFrame(0.f, from);
	placementVisualOffsetAnimation.InsertKeyFrame(1.f, to);
	m_placementVisual.StartAnimation(L"Offset.XY", placementVisualOffsetAnimation);
}

void RoundedAcrylicVisual::Hide()
{
	m_placementVisual.Size({ 1.f, 1.f });
}