#ifndef WDK_GFX__HPP
#define WDK_GFX__HPP

#include <WdkDef.hpp>

namespace Wdk
{
	class IWindow;

	class __declspec(novtable) IGfxDevice
	{
	public:
	};

	IGfxDevice* CreateDevice(VOID);
	VOID        DestroyDevice(IGfxDevice* pDevice);
}

#endif // WDK_GFX__HPP
