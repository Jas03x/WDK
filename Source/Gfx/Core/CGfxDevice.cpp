#include "CGfxDevice.hpp"

using namespace Wdk;

IGfxDevice* Wdk::CreateDevice(HWND hWND, ULONG Width, ULONG Height)
{
	return CGfxDevice::Create(hWND, Width, Height);
}

VOID Wdk::DestroyDevice(IGfxDevice* pDevice)
{
	CGfxDevice::Destroy(static_cast<CGfxDevice*>(pDevice));
}

CGfxDevice* CGfxDevice::Create(HWND hWND, ULONG Width, ULONG Height)
{
	CGfxDevice* pDevice = new CGfxDevice();

	if (pDevice != NULL)
	{
		if (pDevice->Initialize(hWND, Width, Height) == FALSE)
		{
			DestroyDevice(pDevice);
			pDevice = NULL;
		}
	}

	return pDevice;
}

VOID CGfxDevice::Destroy(CGfxDevice* pDevice)
{
	if (pDevice != NULL)
	{
		delete pDevice;
		pDevice = NULL;
	}
}

CGfxDevice::CGfxDevice(VOID)
{

}

CGfxDevice::~CGfxDevice(VOID)
{

}

BOOL CGfxDevice::Initialize(HWND hWND, ULONG Width, ULONG Height)
{
	BOOL Status = TRUE;

	return Status;
}

VOID CGfxDevice::Uninitialize(VOID)
{

}
