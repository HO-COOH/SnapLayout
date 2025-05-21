#pragma once
#include <winrt/base.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d2d1_2.h>


struct DirectXFactory
{
	static winrt::com_ptr<ID3D11Device> d3d11Device;
	static winrt::com_ptr<IDXGIDevice> dxgiDevice;
	static winrt::com_ptr<ID2D1Factory2> d2dFactory;
	static winrt::com_ptr<ID2D1Device> d2dDevice;
	
	static void Init();
};

