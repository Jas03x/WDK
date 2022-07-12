#ifndef WDK_SYS__HPP
#define WDK_SYS__HPP

#include "WdkDef.hpp"

#ifdef CreateWindow
#undef CreateWindow
#endif

namespace Wdk
{
	enum WIN_AREA
	{
		CLIENT = 0,
		WINDOW = 1
	};

	struct WinRect
	{
		UINT x;
		UINT y;
		UINT width;
		UINT height;
	};

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
		virtual BOOL    GetRect(WIN_AREA area, WinRect& rRect) = 0;
	};

	IWindow* CreateWindow(LPCWSTR ClassName, LPCWSTR WindowName, ULONG ClientWidth, ULONG ClientHeight);
	VOID	 DestroyWindow(IWindow* pWindow);
}

#endif // WDK_SYS__HPP