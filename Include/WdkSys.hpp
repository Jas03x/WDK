#ifndef WDK_SYS__HPP
#define WDK_SYS__HPP

#include "WdkDef.hpp"

namespace Wdk
{
	enum WIN_MSG
	{
		INVALID = 0,
		QUIT = 1
	};

	struct WinEvent
	{
		WIN_MSG msg;
	};

	class __declspec(novtable) IWindow
	{
	public:
		virtual HWND	GetHandle(VOID) = 0;
		virtual BOOL	Open(VOID) = 0;
		virtual BOOL	GetEvent(WinEvent& rEvent) = 0;
	};

	IWindow* CreateWindow(LPCWSTR ClassName, LPCWSTR WindowName, ULONG Width, ULONG Height);
	VOID	 DestroyWindow(IWindow* pWindow);
}

#endif // WDK_SYS__HPP