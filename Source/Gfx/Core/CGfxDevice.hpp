#ifndef GFX_DEVICE_HPP
#define GFX_DEVICE_HPP

#include "Wdk.hpp"
#include "WdkGfx.hpp"

namespace Wdk
{
	class CGfxDevice : public Object, public IGfxDevice
	{
	private:
		CGfxDevice(VOID);
		virtual ~CGfxDevice(VOID);

		BOOL Initialize(HWND hWND, ULONG Width, ULONG Height);
		VOID Uninitialize(VOID);

	public:
		static CGfxDevice* Create(HWND hWND, ULONG Width, ULONG Height);
		static VOID        Destroy(CGfxDevice* pDevice);

	public:
	};
}

#endif // GFX_DEVICE_HPP
