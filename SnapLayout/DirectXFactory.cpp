#include "pch.h"
#include "DirectXFactory.h"

winrt::com_ptr<ID3D11Device> DirectXFactory::d3d11Device;
winrt::com_ptr<IDXGIDevice> DirectXFactory::dxgiDevice;
winrt::com_ptr<ID2D1Factory2> DirectXFactory::d2dFactory;
winrt::com_ptr<ID2D1Device> DirectXFactory::d2dDevice;

void DirectXFactory::Init()
{
	//d3d
	winrt::check_hresult(D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		d3d11Device.put(),
		nullptr,
		nullptr
	));
	winrt::check_hresult(d3d11Device->QueryInterface(dxgiDevice.put()));

	//d2d
	winrt::check_hresult(D2D1CreateFactory(
		D2D1_FACTORY_TYPE::D2D1_FACTORY_TYPE_SINGLE_THREADED,
		d2dFactory.put()
	));
	winrt::check_hresult(d2dFactory->CreateDevice(dxgiDevice.get(), d2dDevice.put()));
}
