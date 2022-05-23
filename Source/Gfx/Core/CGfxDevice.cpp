#include "CGfxDevice.hpp"

using namespace Wdk;

IGfxDevice* Wdk::CreateDevice(VOID)
{
	return CGfxDevice::Create();
}

VOID Wdk::DestroyDevice(IGfxDevice* pDevice)
{
	CGfxDevice::Destroy(static_cast<CGfxDevice*>(pDevice));
}

CGfxDevice* CGfxDevice::Create()
{
	CGfxDevice* pDevice = new CGfxDevice();

	if (pDevice != NULL)
	{
		if (pDevice->Initialize() == FALSE)
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

BOOL CGfxDevice::Initialize(VOID)
{
	BOOL Status = TRUE;

	return Status;
}

VOID CGfxDevice::Uninitialize(VOID)
{

}
