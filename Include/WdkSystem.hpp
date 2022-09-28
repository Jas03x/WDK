#ifndef WDK_SYS__HPP
#define WDK_SYS__HPP

#include "WdkDef.hpp"

#ifdef CreateWindow
#undef CreateWindow
#endif

enum WIN_AREA
{
	CLIENT = 0,
	WINDOW = 1
};

struct WIN_RECT
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

struct WIN_EVENT
{
	WIN_MSG msg;
};

class __declspec(novtable) IWindow
{
public:
	virtual HWND	GetHandle(VOID) = 0;
	virtual BOOL	Open(VOID) = 0;
	virtual BOOL	GetEvent(WIN_EVENT& rEvent) = 0;
	virtual BOOL    GetRect(WIN_AREA area, WIN_RECT& rRect) = 0;
};

IWindow* CreateWindow(PCWCHAR ClassName, PCWCHAR WindowName, ULONG ClientWidth, ULONG ClientHeight);
VOID	 DestroyWindow(IWindow* pIWindow);

#endif // WDK_SYS__HPP